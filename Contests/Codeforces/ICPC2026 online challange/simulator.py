import sys
import math
import random
import subprocess
import time

def simulate_test(test_input_str, executable_path):
    # Parse test configuration
    lines = test_input_str.strip().split('\n')
    line1 = lines[0].split()
    K = int(line1[0])
    S = float(line1[1])
    latency = float(line1[2])
    bw = float(line1[3])
    bytes_per_token = int(line1[4])
    num_layers = int(line1[5])
    
    line2 = lines[1].split()
    SLO1 = float(line2[0])
    SLO2 = float(line2[1])
    tp_UB = float(line2[2])
    tp_base = float(line2[3])
    dist_base = float(line2[4])
    w_tp = float(line2[5])
    w_c = float(line2[6])
    
    N = int(lines[2].strip())
    
    # Task time tables
    # batch_size -> (p_pre, p_proc, p_post, d_pre, d_proc, d_post)
    table_rows = []
    for i in range(3, 3 + N):
        parts = lines[i].split()
        bs = int(parts[0])
        vals = [float(x) for x in parts[1:]]
        table_rows.append((bs, vals))
    
    table_rows.sort(key=lambda x: x[0])
    
    def lookup_table(step_idx, size):
        # Step indices: 0: p_pre, 1: p_proc, 2: p_post, 3: d_pre, 4: d_proc, 5: d_post
        valid = [(bs, vals[step_idx]) for bs, vals in table_rows if vals[step_idx] >= 0]
        if not valid:
            return 1.0
        if size <= valid[0][0]:
            return valid[0][1]
        if size >= valid[-1][0]:
            return valid[-1][1]
        for i in range(len(valid) - 1):
            if valid[i][0] <= size <= valid[i+1][0]:
                x0, y0 = valid[i]
                x1, y1 = valid[i+1]
                if x0 == x1: return y0
                return y0 + (y1 - y0) * (size - x0) / (x1 - x0)
        return valid[-1][1]
    
    def transfer_time(length):
        return latency + 8.0 * float(length) * float(bytes_per_token) / (bw * 1e6)
    
    # Parse arrivals
    # format in remaining lines:
    # R (request count)
    # followed by R lines: arr_time Lin Lout
    R_idx = 3 + N
    R = int(lines[R_idx].strip())
    requests = []
    for i in range(R_idx + 1, R_idx + 1 + R):
        parts = lines[i].split()
        arr_t = float(parts[0])
        Lin = int(parts[1])
        Lout = int(parts[2])
        requests.append({'id': i - (R_idx + 1), 'arr': arr_t, 'Lin': Lin, 'Lout': Lout})
    
    # Launch participant process
    proc = subprocess.Popen([executable_path], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True, bufsize=1)
    
    # Send startup config & table
    startup_data = lines[0] + "\n" + lines[1] + "\n" + str(N) + "\n"
    for bs, vals in table_rows:
        startup_data += f"{bs} " + " ".join(f"{v:.9f}" for v in vals) + "\n"
    proc.stdin.write(startup_data)
    proc.stdin.flush()
    
    # Simulation Event Queue: (timestamp, event_type, data)
    # Event types:
    # 0: ARR
    # 1: TDN
    # 2: XDN
    import heapq
    event_q = []
    event_counter = 0
    
    for req in requests:
        heapq.heappush(event_q, (req['arr'], 0, event_counter, ('ARR', req['id'], req['Lin'])))
        event_counter += 1
        
    # State tracking
    req_state = {}
    for req in requests:
        req_state[req['id']] = {
            'arr': req['arr'],
            'Lin': req['Lin'],
            'Lout': req['Lout'],
            'assigned_remote': -1,
            'curr_layer': 0,
            'curr_token': 0,
            'p_post_end': None,
            'token_times': []
        }
        
    E_busy_until = 0.0
    C_busy_until = [0.0] * K
    up_queue_free_at = 0.0
    down_queue_free_at = 0.0
    
    current_time = 0.0
    finished_count = 0
    
    while event_q:
        # Collect all events at exactly the earliest timestamp
        t_next = event_q[0][0]
        current_time = t_next
        
        frame_events = []
        while event_q and abs(event_q[0][0] - t_next) < 1e-9:
            _, _, _, ev = heapq.heappop(event_q)
            frame_events.append(ev)
            
        # Format frame for participant
        frame_str = f"{current_time:.9f}\n{len(frame_events)}\n"
        for ev in frame_events:
            ev_type = ev[0]
            if ev_type == 'ARR':
                frame_str += f"ARR {ev[1]} {ev[2]}\n"
            elif ev_type == 'TDN':
                # TDN server task_spec dur
                frame_str += f"TDN {ev[1]} {ev[2]} {ev[3]:.9f}\n"
            elif ev_type == 'XDN':
                # XDN dir remote size pre_dec m rids...
                # ev: ('XDN', dir, remote, size, pre_dec, m, [rids])
                rids_str = " ".join(str(r) for r in ev[6])
                frame_str += f"XDN {ev[1]} {ev[2]} {ev[3]} {ev[4]} {ev[5]} {rids_str}\n"
            elif ev_type == 'FIN':
                frame_str += f"FIN {ev[1]}\n"
                
        proc.stdin.write(frame_str)
        proc.stdin.flush()
        
        # Read participant response
        resp_line = proc.stdout.readline()
        if not resp_line:
            # Participant closed or crashed
            break
        try:
            tokens = resp_line.strip().split()
            if not tokens:
                num_assignments = 0
            else:
                num_assignments = int(tokens[0])
        except Exception:
            break
            
        for _ in range(num_assignments):
            cmd_line = proc.stdout.readline().strip()
            if not cmd_line: continue
            parts = cmd_line.split()
            server = parts[0]
            task_type = parts[1]
            task_step = parts[2]
            
            if task_type == 'P':
                if task_step == 'PRE':
                    # E P PRE remote rid
                    remote = int(parts[3])
                    rid = int(parts[4])
                    req_state[rid]['assigned_remote'] = remote
                    dur = lookup_table(0, req_state[rid]['Lin'])
                    finish_t = current_time + S + dur
                    # Schedule TDN
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"P PRE {remote} {rid}", dur)))
                    event_counter += 1
                    # Enqueue UP transfer
                    xfer_dur = transfer_time(req_state[rid]['Lin'])
                    xfer_start = max(finish_t, up_queue_free_at)
                    xfer_end = xfer_start + xfer_dur
                    up_queue_free_at = xfer_end
                    size_bytes = req_state[rid]['Lin'] * bytes_per_token
                    heapq.heappush(event_q, (xfer_end, 2, event_counter, ('XDN', 'UP', f"C{remote}", size_bytes, 'PRE', 1, [rid])))
                    event_counter += 1
                elif task_step == 'PROC':
                    # Ck P PROC ls le remote rid
                    ls = int(parts[3])
                    le = int(parts[4])
                    remote = int(parts[5])
                    rid = int(parts[6])
                    req_state[rid]['curr_layer'] = le
                    tot_p_proc = lookup_table(1, req_state[rid]['Lin'])
                    dur = float(le - ls) / float(num_layers) * tot_p_proc
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', f"C{remote}", f"P PROC {ls} {le} {remote} {rid}", dur)))
                    event_counter += 1
                    if le == num_layers:
                        # Enqueue DOWN transfer
                        xfer_dur = transfer_time(req_state[rid]['Lin'])
                        xfer_start = max(finish_t, down_queue_free_at)
                        xfer_end = xfer_start + xfer_dur
                        down_queue_free_at = xfer_end
                        size_bytes = req_state[rid]['Lin'] * bytes_per_token
                        heapq.heappush(event_q, (xfer_end, 2, event_counter, ('XDN', 'DOWN', f"C{remote}", size_bytes, 'PRE', 1, [rid])))
                        event_counter += 1
                elif task_step == 'POST':
                    # E P POST remote rid
                    remote = int(parts[3])
                    rid = int(parts[4])
                    dur = lookup_table(2, req_state[rid]['Lin'])
                    finish_t = current_time + S + dur
                    req_state[rid]['p_post_end'] = finish_t
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"P POST {remote} {rid}", dur)))
                    event_counter += 1
            elif task_type == 'D':
                if task_step == 'PRE':
                    # E D PRE -1 m rid...
                    m = int(parts[4])
                    rids = [int(x) for x in parts[5:5+m]]
                    dur = lookup_table(3, m)
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"D PRE -1 {m} " + " ".join(str(r) for r in rids), dur)))
                    event_counter += 1
                    # Group by remote
                    by_remote = {}
                    for r in rids:
                        rem = req_state[r]['assigned_remote']
                        by_remote.setdefault(rem, []).append(r)
                    for rem in sorted(by_remote.keys()):
                        rem_rids = by_remote[rem]
                        m_k = len(rem_rids)
                        xfer_dur = transfer_time(m_k)
                        xfer_start = max(finish_t, up_queue_free_at)
                        xfer_end = xfer_start + xfer_dur
                        up_queue_free_at = xfer_end
                        size_bytes = m_k * bytes_per_token
                        heapq.heappush(event_q, (xfer_end, 2, event_counter, ('XDN', 'UP', f"C{rem}", size_bytes, 'DEC', m_k, rem_rids)))
                        event_counter += 1
                elif task_step == 'PROC':
                    # Ck D PROC remote m rid...
                    remote = int(parts[3])
                    m = int(parts[4])
                    rids = [int(x) for x in parts[5:5+m]]
                    dur = lookup_table(4, m)
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', f"C{remote}", f"D PROC {remote} {m} " + " ".join(str(r) for r in rids), dur)))
                    event_counter += 1
                    # Enqueue DOWN transfer
                    xfer_dur = transfer_time(m)
                    xfer_start = max(finish_t, down_queue_free_at)
                    xfer_end = xfer_start + xfer_dur
                    down_queue_free_at = xfer_end
                    size_bytes = m * bytes_per_token
                    heapq.heappush(event_q, (xfer_end, 2, event_counter, ('XDN', 'DOWN', f"C{remote}", size_bytes, 'DEC', m, rids)))
                    event_counter += 1
                elif task_step == 'POST':
                    # E D POST -1 m rid...
                    m = int(parts[4])
                    rids = [int(x) for x in parts[5:5+m]]
                    dur = lookup_table(5, m)
                    finish_t = current_time + S + dur
                    heapq.heappush(event_q, (finish_t, 1, event_counter, ('TDN', 'E', f"D POST -1 {m} " + " ".join(str(r) for r in rids), dur)))
                    event_counter += 1
                    for r in rids:
                        req_state[r]['curr_token'] += 1
                        req_state[r]['token_times'].append(finish_t)
                        if req_state[r]['curr_token'] == req_state[r]['Lout']:
                            finished_count += 1
                            heapq.heappush(event_q, (finish_t, 3, event_counter, ('FIN', r)))
                            event_counter += 1
                            
        if finished_count == R and not event_q:
            break
            
    # Send END to participant
    try:
        proc.stdin.write("END\n")
        proc.stdin.flush()
        proc.wait(timeout=2)
    except Exception:
        proc.kill()
        
    # Calculate Score
    # 1. Throughput component
    total_tokens = sum(req_state[r]['Lout'] for r in req_state)
    earliest_arr = min(req_state[r]['arr'] for r in req_state)
    latest_token = max((req_state[r]['token_times'][-1] if req_state[r]['token_times'] else req_state[r]['arr'] + 1000) for r in req_state)
    T_elapsed = max(0.001, latest_token - earliest_arr)
    tp = float(total_tokens) / T_elapsed
    
    if tp_UB > tp_base:
        comp_tp = max(0.0, min(1.0, (tp - tp_base) / (tp_UB - tp_base)))
    else:
        comp_tp = 1.0 if tp >= tp_UB else 0.0
        
    # 2. Waiting-time component
    tdr_list = [req_state[r]['p_post_end'] - req_state[r]['arr'] for r in req_state if req_state[r]['p_post_end'] is not None]
    mean_tdr = sum(tdr_list) / len(tdr_list) if tdr_list else 1e9
    
    tpot_gaps = []
    for r in req_state:
        tt = req_state[r]['token_times']
        if len(tt) > 1:
            for j in range(len(tt) - 1):
                tpot_gaps.append(tt[j+1] - tt[j])
    mean_tpot = sum(tpot_gaps) / len(tpot_gaps) if tpot_gaps else 0.0
    
    excess_tdr = max(0.0, (mean_tdr - SLO1) / SLO1) if SLO1 > 0 else 0.0
    excess_tpot = max(0.0, (mean_tpot - SLO2) / SLO2) if SLO2 > 0 else 0.0
    dist = math.sqrt(excess_tdr**2 + excess_tpot**2)
    
    if dist_base > 0:
        comp_c = max(0.0, 1.0 - dist / dist_base)
    else:
        comp_c = 1.0 if dist == 0.0 else 0.0
        
    final_score = 1000.0 * (w_tp * comp_tp + w_c * comp_c)
    return {
        'score': final_score,
        'tp': tp,
        'comp_tp': comp_tp,
        'mean_tdr': mean_tdr,
        'mean_tpot': mean_tpot,
        'dist': dist,
        'comp_c': comp_c,
        'makespan': T_elapsed
    }

if __name__ == "__main__":
    test_file = sys.argv[1]
    exe_path = sys.argv[2]
    with open(test_file, 'r') as f:
        data = f.read()
    res = simulate_test(data, exe_path)
    print(f"Score: {res['score']:.3f} (tp_comp: {res['comp_tp']:.3f}, wait_comp: {res['comp_c']:.3f}, TDR: {res['mean_tdr']:.1f}ms, TPOT: {res['mean_tpot']:.1f}ms, makespan: {res['makespan']:.1f}ms)")
