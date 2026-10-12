import subprocess

code = open("best_15630.cpp").read()

decl = "    int pending_down_dec = 0;"
new_decl = """    int pending_down_dec = 0;
    double up_free_time = 0.0;
    double down_free_time = 0.0;"""
code = code.replace(decl, new_decl)

old_dpre = """                    } else if (task2 == "PRE") {
                        int minus1, m;
                        cin >> minus1 >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) reqs[rid].state = WAITING_UP_DEC;"""
new_dpre = """                    } else if (task2 == "PRE") {
                        int minus1, m;
                        cin >> minus1 >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        
                        int r_counts[8] = {0};
                        for (int rid : rids) r_counts[reqs[rid].assigned_remote]++;
                        for (int k = 0; k < K; ++k) {
                            if (r_counts[k] > 0) {
                                up_free_time = max(current_time, up_free_time) + transfer_time(r_counts[k]);
                            }
                        }
                        
                        for (int rid : rids) reqs[rid].state = WAITING_UP_DEC;"""
code = code.replace(old_dpre, new_dpre)

old_ppre = """                    if (task2 == "PRE") {
                        int remote, rid; double dur;
                        cin >> remote >> rid >> dur;
                        reqs[rid].state = WAITING_UP_PRE;"""
new_ppre = """                    if (task2 == "PRE") {
                        int remote, rid; double dur;
                        cin >> remote >> rid >> dur;
                        up_free_time = max(current_time, up_free_time) + transfer_time(reqs[rid].L_in);
                        reqs[rid].state = WAITING_UP_PRE;"""
code = code.replace(old_ppre, new_ppre)

old_pproc = """                    } else if (task2 == "PROC") {
                        int ls, le, remote, rid; double dur;
                        cin >> ls >> le >> remote >> rid >> dur;
                        if (task1 == "P") {
                            reqs[rid].current_layer = le;
                            if (le == num_layers) {
                                reqs[rid].state = READY_P_POST;
                            } else {"""
new_pproc = """                    } else if (task2 == "PROC") {
                        int ls, le, remote, rid; double dur;
                        cin >> ls >> le >> remote >> rid >> dur;
                        if (task1 == "P") {
                            reqs[rid].current_layer = le;
                            if (le == num_layers) {
                                reqs[rid].state = WAITING_DOWN_PRE;
                                down_free_time = max(current_time, down_free_time) + transfer_time(reqs[rid].L_in);
                            } else {"""
code = code.replace(old_pproc, new_pproc)

old_dproc = """                    } else if (task2 == "PROC") {
                        int remote, m;
                        cin >> remote >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;"""
new_dproc = """                    } else if (task2 == "PROC") {
                        int remote, m;
                        cin >> remote >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;
                        down_free_time = max(current_time, down_free_time) + transfer_time(m);"""
code = code.replace(old_dproc, new_dproc)

old_idle = """            bool idle_for_down = (pending_down_dec > 0 && w_c >= 0.50);"""
new_idle = """            bool idle_for_down = false;
            if (pending_down_dec > 0) {
                if (w_c >= 0.50) idle_for_down = true; 
                else {
                    double time_until_dpost = down_free_time - current_time;
                    if (time_until_dpost < S + 5.0) {
                        idle_for_down = true;
                    }
                }
            }"""
code = code.replace(old_idle, new_idle)

with open("test_tracker2.cpp", "w") as f:
    f.write(code)

subprocess.run("g++ -std=c++17 -O2 -o test_tracker2 test_tracker2.cpp", shell=True)
res = subprocess.run("python3 benchmark.py best_15630 test_tracker2", shell=True, stdout=subprocess.PIPE, text=True)
print(res.stdout)
