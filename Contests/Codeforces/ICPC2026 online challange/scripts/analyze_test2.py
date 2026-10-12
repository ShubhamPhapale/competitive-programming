import sys
import subprocess
import heapq
import math

def trace_execution(test_file, exe_path):
    with open(test_file, 'r') as f:
        data = f.read()
        
    lines = data.strip().split('\n')
    line1 = lines[0].split()
    K = int(line1[0])
    S = float(line1[1])
    latency = float(line1[2])
    bw = float(line1[3])
    bytes_per_token = int(line1[4])
    num_layers = int(line1[5])
    
    line2 = lines[1].split()
    w_tp = float(line2[5])
    w_c = float(line2[6])
    
    N = int(lines[2].strip())
    table_rows = []
    for i in range(3, 3 + N):
        parts = lines[i].split()
        bs = int(parts[0])
        vals = [float(x) for x in parts[1:]]
        table_rows.append((bs, vals))
    table_rows.sort(key=lambda x: x[0])
    
    def lookup_table(step_idx, size):
        valid = [(bs, vals[step_idx]) for bs, vals in table_rows if vals[step_idx] >= 0]
        if not valid: return 1.0
        if size <= valid[0][0]: return valid[0][1]
        if size >= valid[-1][0]: return valid[-1][1]
        for i in range(len(valid) - 1):
            if valid[i][0] <= size <= valid[i+1][0]:
                x0, y0 = valid[i]
                x1, y1 = valid[i+1]
                if x0 == x1: return y0
                return y0 + (y1 - y0) * (size - x0) / (x1 - x0)
        return valid[-1][1]
        
    def transfer_time(length):
        return latency + 8.0 * float(length) * float(bytes_per_token) / (bw * 1e6)
        
    R_idx = 3 + N
    R = int(lines[R_idx].strip())
    req_state = {}
    arrivals = []
    for i in range(R_idx + 1, R_idx + 1 + R):
        rid = i - (R_idx + 1)
        arr_t, Lin, Lout = lines[i].split()
        arr_t = float(arr_t)
        Lin = int(Lin)
        Lout = int(Lout)
        req_state[rid] = {'arr': arr_t, 'Lin': Lin, 'Lout': Lout, 'p_post_end': None, 'token_times': []}
        arrivals.append((arr_t, rid, Lin))
    arrivals.sort(key=lambda x: x[0])
    
    proc = subprocess.Popen([exe_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    header = f"{K} {S:.9f} {latency:.9f} {bw:.9f} {bytes_per_token} {num_layers}\n"
    header += f"{lines[1]}\n{N}\n"
    for bs, vals in table_rows:
        header += f"{bs} " + " ".join(f"{v:.9f}" for v in vals) + "\n"
    proc.stdin.write(header)
    proc.stdin.flush()
    
    arr_idx = 0
    event_q = []
    current_time = 0.0
    event_counter = 0
    
    if arrivals:
        current_time = arrivals[0][0]
        batch_arr = []
        while arr_idx < len(arrivals) and arrivals[arr_idx][0] == current_time:
            batch_arr.append(arrivals[arr_idx])
            arr_idx += 1
        for arr_t, rid, Lin in batch_arr:
            heapq.heappush(event_q, (current_time, 0, event_counter, ('ARR', rid, Lin)))
            event_counter += 1
            
    up_queue_free_at = 0.0
    down_queue_free_at = 0.0
    finished_count = 0
    d_pre_count = 0
    d_post_count = 0
    last_p_post_time = 0.0
    
    while True:
        if not event_q:
            if arr_idx < len(arrivals):
                current_time = arrivals[arr_idx][0]
                while arr_idx < len(arrivals) and arrivals[arr_idx][0] == current_time:
                    heapq.heappush(event_q, (current_time, 0, event_counter, ('ARR', arrivals[arr_idx][1], arrivals[arr_idx][2])))
                    event_counter += 1
                    arr_idx += 1
            else:
                break
                
        current_time = event_q[0][0]
        events_this_frame = []
        while event_q and event_q[0][0] == current_time:
            item = heapq.heappop(event_q)
            events_this_frame.append(item[3])
            
        frame_msg = f"{current_time:.9f}\n{len(events_this_frame)}\n"
        for ev in events_this_frame:
            ev_type = ev[0]
            if ev_type == 'ARR':
                frame_msg += f"ARR {ev[1]} {ev[2]}\n"
            elif ev_type == 'TDN':
                frame_msg += f"TDN {ev[1]} {ev[2]} {ev[3]:.9f}\n"
            elif ev_type == 'XDN':
                frame_msg += f"XDN {ev[1]} {ev[2]} {ev[3]} {ev[4]} {ev[5]} " + " ".join(str(r) for r in ev[6]) + "\n"
            elif ev_type == 'FIN':
                frame_msg += f"FIN {ev[1]}\n"
                
        proc.stdin.write(frame_msg)
        proc.stdin.flush()
        
        assign_line = proc.stdout.readline()
        if not assign_line: break
        k_assigns = int(assign_line.strip())
        
        for _ in range(k_assigns):
            cmd = proc.stdout.readline().strip()
            parts = cmd.split()
            server = parts[0]
            task_type = parts[1]
            task_step = parts[2]
            
            if task_type == 'P':
                if task_step == 'PRE':
                    remote = int(parts[3])
                    rid = int(parts[4])
                    Lin = req_state[rid]['Lin']
                    dur = lookup_table(0, Lin)
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"P PRE {remote} {rid}", dur)))
                    event_counter += 1
                    xfer_dur = transfer_time(Lin)
                    xfer_start = max(finish_t, up_queue_free_at)
                    xfer_end = xfer_start + xfer_dur
                    up_queue_free_at = xfer_end
                    size_bytes = Lin * bytes_per_token
                    heapq.heappush(event_q, (xfer_end, 2, event_counter, ('XDN', 'UP', f"C{remote}", size_bytes, 'PRE', 1, [rid])))
                    event_counter += 1
                elif task_step == 'PROC':
                    ls = int(parts[3])
                    le = int(parts[4])
                    remote = int(parts[5])
                    rid = int(parts[6])
                    Lin = req_state[rid]['Lin']
                    dur = lookup_table(1, Lin) * (le - ls) / float(num_layers)
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', f"C{remote}", f"P PROC {ls} {le} {remote} {rid}", dur)))
                    event_counter += 1
                    if le == num_layers:
                        xfer_dur = transfer_time(Lin)
                        xfer_start = max(finish_t, down_queue_free_at)
                        xfer_end = xfer_start + xfer_dur
                        down_queue_free_at = xfer_end
                        size_bytes = Lin * bytes_per_token
                        heapq.heappush(event_q, (xfer_end, 2, event_counter, ('XDN', 'DOWN', f"C{remote}", size_bytes, 'PRE', 1, [rid])))
                        event_counter += 1
                elif task_step == 'POST':
                    remote = int(parts[3])
                    rid = int(parts[4])
                    Lin = req_state[rid]['Lin']
                    dur = lookup_table(2, Lin)
                    finish_t = current_time + S + dur
                    req_state[rid]['p_post_end'] = finish_t
                    last_p_post_time = max(last_p_post_time, finish_t)
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"P POST {remote} {rid}", dur)))
                    event_counter += 1
            elif task_type == 'D':
                if task_step == 'PRE':
                    d_pre_count += 1
                    m = int(parts[4])
                    rids = [int(x) for x in parts[5:5+m]]
                    dur = lookup_table(3, m)
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"D PRE -1 {m} " + " ".join(str(r) for r in rids), dur)))
                    event_counter += 1
                    remote_buckets = {}
                    for r in rids:
                        remote_buckets.setdefault(0, []).append(r)
                    # Note: remote assignments
                elif task_step == 'POST':
                    d_post_count += 1
                    m = int(parts[4])
                    rids = [int(x) for x in parts[5:5+m]]
                    dur = lookup_table(5, m)
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"D POST -1 {m} " + " ".join(str(r) for r in rids), dur)))
                    event_counter += 1
                    for r in rids:
                        req_state[r]['token_times'].append(finish_t)
                        if len(req_state[r]['token_times']) == req_state[r]['Lout']:
                            finished_count += 1
                            heapq.heappush(event_q, (finish_t, 3, event_counter, ('FIN', r)))
                            event_counter += 1
                            
        if finished_count == R and not event_q:
            break
            
    print(f"Test finished at t = {current_time:.1f} ms")
    print(f"Last P POST completed at t = {last_p_post_time:.1f} ms")
    print(f"Total D PRE waves: {d_pre_count}, D POST waves: {d_post_count}")

if __name__ == "__main__":
    trace_execution("local_tests/test_2.in", "./solution")
