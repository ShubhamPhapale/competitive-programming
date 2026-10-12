import subprocess

code = open("best_15630.cpp").read()

# Add tracking for decode wave start and prediction
decl = "    int pending_down_dec = 0;"
new_decl = """    int pending_down_dec = 0;
    double current_wave_start = 0;
    double expected_wave_duration = 0;"""

code = code.replace(decl, new_decl)

old_dpre = """            if (!scheduled && !ready_d_pre.empty()) {
                stringstream ss;
                ss << "E D PRE -1 " << ready_d_pre.size();"""

new_dpre = """            if (!scheduled && !ready_d_pre.empty()) {
                stringstream ss;
                int b_size = ready_d_pre.size();
                ss << "E D PRE -1 " << b_size;
                current_wave_start = current_time;
                
                int remotes_used = min(b_size, K);
                int b_per_remote = (b_size + remotes_used - 1) / remotes_used;
                double t_e = 2.0 * S + d_pre_tab.lookup(b_size) + d_post_tab.lookup(b_size);
                double t_up = remotes_used * transfer_time(b_per_remote);
                double t_remote = S + d_proc_tab.lookup(b_per_remote);
                double t_down = remotes_used * transfer_time(b_per_remote);
                expected_wave_duration = t_e + t_up + t_remote + t_down - d_post_tab.lookup(b_size) - S;
                // expected_wave_duration is time until D POST is ready!"""

code = code.replace(old_dpre, new_dpre)

# Change idle_for_down
old_idle = """            bool idle_for_down = (pending_down_dec > 0 && w_c >= 0.85);"""
new_idle = """            bool idle_for_down = false;
            if (pending_down_dec > 0) {
                if (w_c >= 0.85) idle_for_down = true;
                else {
                    // Predictive scheduling!
                    double time_until_dpost = (current_wave_start + expected_wave_duration) - current_time;
                    // Check if starting a P PRE would block D POST
                    // We dont know exactly which P PRE, but S is at least S.
                    if (time_until_dpost < S + 5.0) { // 5.0 is a conservative p_pre bound
                        idle_for_down = true;
                    }
                }
            }"""

code = code.replace(old_idle, new_idle)

with open("test_predictive.cpp", "w") as f:
    f.write(code)

subprocess.run("g++ -std=c++17 -O2 -o test_predictive test_predictive.cpp", shell=True)
res = subprocess.run("python3 benchmark.py best_15630 test_predictive", shell=True, stdout=subprocess.PIPE, text=True)
print(res.stdout)
