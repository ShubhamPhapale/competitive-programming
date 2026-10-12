import subprocess, re

code = open("best_15630.cpp").read()

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
                int batch = min((int)ready_d_pre.size(), max(1, cap));
                stringstream ss;
                ss << "E D PRE -1 " << batch;
                for (int i = 0; i < batch; ++i) {{
                    int rid = ready_d_pre[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }}"""
    modified = code.replace(old_dpre, new_dpre)
    fname = f"test_dcap_{label}"
    with open(f"{fname}.cpp", "w") as f:
        f.write(modified)
    subprocess.run(f"g++ -std=c++17 -O2 -o {fname} {fname}.cpp", shell=True)
    res = subprocess.run(f"python3 benchmark.py best_15630 {fname}", shell=True, 
                         stdout=subprocess.PIPE, text=True)
    total_delta = 0.0
    reg_sum = 0.0
    max_loss = 0.0
    losses = 0
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
        if "LOSS" in line:
            losses += 1
    print(f"Cap={label:30s} Delta: {total_delta:+9.1f}  Reg: {reg_sum:+8.1f}  MaxLoss: {max_loss:+7.1f}  #Loss: {losses}")
    return total_delta, reg_sum

# Dynamic caps based on system parameters
# The idea: optimal batch size B* should satisfy T_cycle(B*) <= SLO2 
# where T_cycle = 2S + d_pre(B) + d_post(B)  (E time dominates for small B)
# So B* ~ SLO2 / (something dependent on S and task times)

# Simple heuristics:
test_cap("3*K", "3K_baseline")
test_cap("(int)(K * max(2.0, S))", "K_x_maxS2")
test_cap("(int)(K * max(3.0, S/2.0))", "K_x_max3_Sd2")
test_cap("(int)(SLO2 > 10 ? 4*K : 3*K)", "4K_if_hSLO")
test_cap("(int)(S >= 5 ? 5*K : 3*K)", "5K_if_hS")
test_cap("(int)(S >= 3 ? 4*K : 3*K)", "4K_if_S3")
# Smart: use SLO2 to determine cap
test_cap("max(3*K, (int)(SLO2 / (2*S + 0.5)))", "SLO2_based")
# Another: cap based on both S and K
test_cap("max(3*K, (int)(S * K))", "SK_floor")

