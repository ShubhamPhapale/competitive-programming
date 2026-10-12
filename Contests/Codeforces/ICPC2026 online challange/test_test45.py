import subprocess, re

code = open("best_15630.cpp").read()

old_dpre = """            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                stringstream ss;
                ss << "E D PRE -1 " << ready_d_pre.size();
                for (int rid : ready_d_pre) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }"""

for cap_val in [24, 32, 40, 48, 64, 80, 100, 200, 500, 1000]:
    new_dpre = f"""            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {{
                int cap = {cap_val};
                int batch = min((int)ready_d_pre.size(), max(1, cap));
                stringstream ss;
                ss << "E D PRE -1 " << batch;
                for (int i = 0; i < batch; ++i) {{
                    int rid = ready_d_pre[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }}"""
    modified = code.replace(old_dpre, new_dpre)
    fname = f"test_t45_{cap_val}"
    with open(f"{fname}.cpp", "w") as f:
        f.write(modified)
    subprocess.run(f"g++ -std=c++17 -O2 -o {fname} {fname}.cpp", shell=True)
    res = subprocess.run(f"python3 simulator.py local_tests/test_45.in ./{fname}", 
                         shell=True, stdout=subprocess.PIPE, text=True)
    score = 0.0
    for line in res.stdout.splitlines():
        if "Score:" in line:
            try: score = float(line.split("Score:")[1].split()[0])
            except: pass
    print(f"Cap={cap_val:4d}  Score: {score:.3f}")

