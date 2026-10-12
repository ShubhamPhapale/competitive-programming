import subprocess, re

code = open("best_15630.cpp").read()

# Change 1: Variant D for hold_d_post - don't hold when throughput dominates
old_hold = "            bool hold_d_post = (pending_down_dec > 0);"
new_hold = "            bool hold_d_post = (pending_down_dec > 0 && w_tp < 0.50);"
code = code.replace(old_hold, new_hold)

# Change 2: Add down_free_time tracking
old_decl = "    int pending_down_dec = 0;"
new_decl = """    int pending_down_dec = 0;
    double down_free_time = 0.0;"""
code = code.replace(old_decl, new_decl)

# Change 3: Track down_free_time in D PROC TDN
old_dproc = """                        for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;
                        pending_down_dec++;"""
new_dproc = """                        for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;
                        down_free_time = max(current_time, down_free_time) + transfer_time(m);
                        pending_down_dec++;"""
code = code.replace(old_dproc, new_dproc)

# Change 4: Predictive idle gated on w_c < 0.25 (RCA finding: regressions at w_c >= 0.3)
old_idle = "                bool idle_for_down = (pending_down_dec > 0 && w_c >= 0.50);"
new_idle = """                bool idle_for_down = false;
                if (pending_down_dec > 0) {
                    if (w_c >= 0.50) idle_for_down = true;
                    else if (w_c < 0.25) {
                        double time_until_dpost = down_free_time - current_time;
                        if (time_until_dpost < S * 1.5) idle_for_down = true;
                    }
                }"""
code = code.replace(old_idle, new_idle)

with open("test_combined_v1.cpp", "w") as f:
    f.write(code)

subprocess.run("g++ -std=c++17 -O2 -o test_combined_v1 test_combined_v1.cpp", shell=True)
res = subprocess.run("python3 benchmark.py best_15630 test_combined_v1", shell=True, 
                     stdout=subprocess.PIPE, text=True)
print(res.stdout[-3000:])  # Last 3000 chars for summary
