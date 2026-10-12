import subprocess, re

code = open("best_15630.cpp").read()

# Current D PRE dispatches ALL ready_d_pre at once.
# Test: cap the D PRE batch size at various levels.

old_dpre = """            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                stringstream ss;
                ss << "E D PRE -1 " << ready_d_pre.size();
                for (int rid : ready_d_pre) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }"""

def test_cap(cap_expr, label):
    new_dpre = f"""            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {{
                int cap = {cap_expr};
                int batch = min((int)ready_d_pre.size(), cap);
                stringstream ss;
                ss << "E D PRE -1 " << batch;
                for (int i = 0; i < batch; ++i) {{
                    int rid = ready_d_pre[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }}"""
    modified = code.replace(old_dpre, new_dpre)
    fname = f"test_cap_{label}"
    with open(f"{fname}.cpp", "w") as f:
        f.write(modified)
    subprocess.run(f"g++ -std=c++17 -O2 -o {fname} {fname}.cpp", shell=True)
    res = subprocess.run(f"python3 benchmark.py best_15630 {fname}", shell=True, 
                         stdout=subprocess.PIPE, text=True)
    total_delta = 0.0
    reg_sum = 0.0
    for line in res.stdout.split('\n'):
        if "Total Sum of Scores" in line:
            m = re.search(r'\(\+?\s*([-0-9.]+)\s*pts total delta\)', line)
            if m: total_delta = float(m.group(1))
        if "Total Regression Sum" in line:
            m = re.search(r':\s*([-0-9.]+)\s*pts', line)
            if m: reg_sum = float(m.group(1))
    print(f"Cap={label:20s} Delta: {total_delta:+8.3f}  Regression: {reg_sum:+8.3f}")
    return total_delta, reg_sum

# Test various caps
test_cap("K", "K")
test_cap("2*K", "2K")
test_cap("3*K", "3K")
test_cap("K*K", "KK")
test_cap("max(K, 4)", "maxK4")
test_cap("max(K, 8)", "maxK8")
