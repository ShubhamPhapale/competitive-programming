import subprocess, re

code = open("best_15630.cpp").read()

# Test: also cap D POST at 3K
old_dpost = """            if (!ready_d_post.empty() && !hold_d_post) {
                stringstream ss;
                ss << "E D POST -1 " << ready_d_post.size();
                for (int rid : ready_d_post) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_POST;
                }"""

# Also need the D PRE cap
old_dpre = """            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                stringstream ss;
                ss << "E D PRE -1 " << ready_d_pre.size();
                for (int rid : ready_d_pre) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }"""

def test_combo(dpre_cap, dpost_cap, label):
    c = code
    
    new_dpre = f"""            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {{
                int cap = {dpre_cap};
                int batch = min((int)ready_d_pre.size(), max(1, cap));
                stringstream ss;
                ss << "E D PRE -1 " << batch;
                for (int i = 0; i < batch; ++i) {{
                    int rid = ready_d_pre[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }}"""
    c = c.replace(old_dpre, new_dpre)
    
    if dpost_cap:
        new_dpost = f"""            if (!ready_d_post.empty() && !hold_d_post) {{
                int cap_dp = {dpost_cap};
                int batch_dp = min((int)ready_d_post.size(), max(1, cap_dp));
                stringstream ss;
                ss << "E D POST -1 " << batch_dp;
                for (int i = 0; i < batch_dp; ++i) {{
                    int rid = ready_d_post[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_POST;
                }}"""
        c = c.replace(old_dpost, new_dpost)
    
    fname = f"test_combo_{label}"
    with open(f"{fname}.cpp", "w") as f:
        f.write(c)
    subprocess.run(f"g++ -std=c++17 -O2 -o {fname} {fname}.cpp", shell=True)
    res = subprocess.run(f"python3 benchmark.py best_15630 {fname}", shell=True, 
                         stdout=subprocess.PIPE, text=True)
    total_delta = 0.0
    reg_sum = 0.0
    max_loss = 0.0
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
    print(f"Combo={label:25s} Delta: {total_delta:+9.1f}  Reg: {reg_sum:+8.1f}  MaxL: {max_loss:+7.1f}")

# Test D PRE only vs D PRE + D POST cap
test_combo("3*K", None, "dpre3K_only")
test_combo("3*K", "3*K", "dpre3K_dpost3K")
test_combo("3*K", "2*K", "dpre3K_dpost2K")
test_combo("2*K", "2*K", "dpre2K_dpost2K")
test_combo("2*K", "3*K", "dpre2K_dpost3K")

