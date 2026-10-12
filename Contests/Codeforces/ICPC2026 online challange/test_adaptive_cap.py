import subprocess, re

code = open("best_15630.cpp").read()

old_dpre = """            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                stringstream ss;
                ss << "E D PRE -1 " << ready_d_pre.size();
                for (int rid : ready_d_pre) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }"""

old_dpost = """            if (!ready_d_post.empty() && !hold_d_post) {
                stringstream ss;
                ss << "E D POST -1 " << ready_d_post.size();
                for (int rid : ready_d_post) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_POST;
                }"""

def test_cap(cap_expr, label, also_dpost=False):
    c = code
    new_dpre = f"""            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {{
                int cap = {cap_expr};
                int batch = min((int)ready_d_pre.size(), max(1, cap));
                stringstream ss;
                ss << "E D PRE -1 " << batch;
                for (int i = 0; i < batch; ++i) {{
                    int rid = ready_d_pre[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }}"""
    c = c.replace(old_dpre, new_dpre)
    
    if also_dpost:
        new_dpost = f"""            if (!ready_d_post.empty() && !hold_d_post) {{
                int cap_dp = {cap_expr};
                int batch_dp = min((int)ready_d_post.size(), max(1, cap_dp));
                stringstream ss;
                ss << "E D POST -1 " << batch_dp;
                for (int i = 0; i < batch_dp; ++i) {{
                    int rid = ready_d_post[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_POST;
                }}"""
        c = c.replace(old_dpost, new_dpost)
    
    fname = f"test_acap_{label}"
    with open(f"{fname}.cpp", "w") as f:
        f.write(c)
    subprocess.run(f"g++ -std=c++17 -O2 -o {fname} {fname}.cpp", shell=True)
    res = subprocess.run(f"python3 benchmark.py best_15630 {fname}", shell=True, 
                         stdout=subprocess.PIPE, text=True)
    total_delta = 0.0; reg_sum = 0.0; max_loss = 0.0; losses = 0
    regressed = ""
    for line in res.stdout.split('\n'):
        if "Total Sum of Scores" in line:
            m = re.search(r'\(\+?\s*([-0-9.]+)\s*pts total delta\)', line)
            if m: total_delta = float(m.group(1))
        if "Total Regression Sum" in line:
            m = re.search(r':\s*([-0-9.]+)\s*pts', line)
            if m: reg_sum = float(m.group(1))
        if "Max Individual Loss" in line:
            m = re.search(r':\s*([-0-9.]+)\s*pts', line)
            if m: max_loss = float(m.group(1))
        if "LOSS" in line: losses += 1
        if "Regressed" in line: regressed = line.strip()
    print(f"Cap={label:35s} Delta: {total_delta:+9.1f}  Reg: {reg_sum:+8.1f}  MaxL: {max_loss:+7.1f}  #L: {losses}")
    if regressed: print(f"  {regressed}")
    return total_delta, reg_sum

# Key insight: test #45 needs cap >= 40 (5K=40), test #6 needs cap >= 12 (3K=12)
# But test #45 has S=1.2 and test #6 has S=10 
# For high-latency tests (like #24), cap should be very high (no effective cap)
# Strategy: use transfer_time to determine if capping is beneficial

# Approaches:
# 1. Use a higher cap (4K, 5K) to avoid test #45 regression
test_cap("4*K", "4K", True)
test_cap("5*K", "5K", True)

# 2. Make cap conditional on latency/bandwidth
# If high latency, don't cap (the network dominates, so batching helps)
test_cap("(latency > 10 ? 1000 : 3*K)", "3K_unless_hlat", True)
test_cap("(latency > 10 || bw < 5 ? 1000 : 3*K)", "3K_unless_slow", True)

# 3. Make cap conditional on S
# High S = big scheduling cost = needs bigger batches to amortize
test_cap("(int)(3*K + max(0.0, (S-2.0)) * K)", "3K_plus_S", True)

# 4. Use transfer time as a signal
# If transfer_time(1) > SLO2/2, network is the bottleneck, don't cap
test_cap("(transfer_time(1) > SLO2/2 ? 1000 : 3*K)", "3K_unless_netbot", True)

