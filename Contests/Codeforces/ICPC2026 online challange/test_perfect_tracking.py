import subprocess

code = open("best_15630.cpp").read()

decl = "    int pending_down_dec = 0;"
new_decl = """    int pending_down_dec = 0;
    double up_free_time = 0;
    double down_free_time = 0;"""
code = code.replace(decl, new_decl)

# Intercept TDN events to update up_free_time and down_free_time
# TDN E P PRE:
old_tdn_ppre = """                    if (task2 == "PRE") {
                        int minus1, m, best;
                        cin >> minus1 >> m >> best;
                        double dur; cin >> dur;"""
new_tdn_ppre = """                    if (task2 == "PRE") {
                        int minus1, m, best;
                        cin >> minus1 >> m >> best;
                        double dur; cin >> dur;
                        up_free_time = max(current_time, up_free_time) + transfer_time(reqs[best].L_in);"""
code = code.replace(old_tdn_ppre, new_tdn_ppre)

# TDN E D PRE:
old_tdn_dpre = """                    } else if (task2 == "PRE") {
                        int minus1, m;
                        cin >> minus1 >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;"""
new_tdn_dpre = """                    } else if (task2 == "PRE") {
                        int minus1, m;
                        cin >> minus1 >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        // Find distinct remotes
                        int r_counts[8] = {0};
                        int distinct = 0;
                        for(int rid : rids) {
                            if (r_counts[reqs[rid].assigned_remote] == 0) distinct++;
                            r_counts[reqs[rid].assigned_remote]++;
                        }
                        for(int k=0; k<K; ++k) {
                            if (r_counts[k] > 0) {
                                up_free_time = max(current_time, up_free_time) + transfer_time(r_counts[k]);
                            }
                        }"""
code = code.replace(old_tdn_dpre, new_tdn_dpre)

# TDN Ck P PROC:
old_tdn_pproc = """                        cin >> ls >> le >> remote >> rid >> dur;
                        if (task1 == "P") {
                            reqs[rid].current_layer = le;
                            if (le == num_layers) {
                                reqs[rid].state = READY_P_POST;
                            } else {"""
new_tdn_pproc = """                        cin >> ls >> le >> remote >> rid >> dur;
                        if (task1 == "P") {
                            reqs[rid].current_layer = le;
                            if (le == num_layers) {
                                reqs[rid].state = READY_P_POST;
                                down_free_time = max(current_time, down_free_time) + transfer_time(reqs[rid].L_in);
                            } else {"""
code = code.replace(old_tdn_pproc, new_tdn_pproc)

# TDN Ck D PROC:
old_tdn_dproc = """                    } else if (task2 == "PROC") {
                        int remote, m;
                        cin >> remote >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;"""
new_tdn_dproc = """                    } else if (task2 == "PROC") {
                        int remote, m;
                        cin >> remote >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;
                        down_free_time = max(current_time, down_free_time) + transfer_time(m);"""
code = code.replace(old_tdn_dproc, new_tdn_dproc)

# Now, implement predictive idle for down!
old_idle = """            bool idle_for_down = (pending_down_dec > 0 && w_c >= 0.85);"""
new_idle = """            bool idle_for_down = false;
            // If any down transfers for decode are pending, or D PROC is currently running and will finish soon
            if (decode_progressing > 0) {
                // We know exactly when the decode down transfers will arrive!
                // Actually, if pending_down_dec > 0, they are already in the queue.
                // Their arrival time is down_free_time!
                double time_until_dpost = down_free_time - current_time;
                if (pending_down_dec == 0) {
                    // D PROC hasn't finished yet. It will finish at some C_free time.
                    // Roughly, it will add to down_free_time.
                    // For now, let's just use pending_down_dec or down_free_time.
                    time_until_dpost = 9999.0;
                }
                
                if (pending_down_dec > 0) {
                    if (w_c >= 0.85) idle_for_down = true; // baseline behavior
                    else if (time_until_dpost < S + 5.0) {
                        // If it's arriving very soon, don't start P PRE!
                        idle_for_down = true;
                    }
                }
            }"""
code = code.replace(old_idle, new_idle)

with open("test_tracker.cpp", "w") as f:
    f.write(code)

subprocess.run("g++ -std=c++17 -O2 -o test_tracker test_tracker.cpp", shell=True)
subprocess.run("python3 benchmark.py best_15630 test_tracker", shell=True)
