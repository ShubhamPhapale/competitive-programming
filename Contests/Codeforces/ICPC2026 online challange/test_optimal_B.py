import subprocess, re

code = open("best_15630.cpp").read()

# Insert a function that computes optimal batch size using the task-time table
# This function uses the ACTUAL task times to find B* that maximizes B/T_cycle
# while keeping TPOT <= SLO2 if w_c > 0.

optimal_fn = """
int compute_optimal_B() {
    int best_B = 3 * K;  // default
    double best_score = -1e18;
    
    for (int B = 1; B <= min(200, 10 * K); ++B) {
        int remotes_used = min(B, K);
        int b_per_remote = (B + remotes_used - 1) / remotes_used;
        
        double t_e = 2.0 * S + d_pre_tab.lookup(B) + d_post_tab.lookup(B);
        double t_up = 0;
        for (int k = 0; k < remotes_used; ++k) {
            t_up += transfer_time(b_per_remote);
        }
        double t_remote = S + d_proc_tab.lookup(b_per_remote);
        double t_down = t_up;  // same formula
        
        double t_cycle = max({t_e, t_up, t_remote, t_down});
        double throughput = (double)B / t_cycle;
        
        // Score = w_tp * throughput_gain - w_c * tpot_penalty
        double tp_score = throughput;
        
        // TPOT penalty: if t_cycle > SLO2, we're over target
        double tpot_penalty = 0;
        if (SLO2 > 0 && t_cycle > SLO2) {
            tpot_penalty = (t_cycle - SLO2) / SLO2;
        }
        
        double score = w_tp * tp_score - w_c * tpot_penalty * 0.1;
        if (score > best_score) {
            best_score = score;
            best_B = B;
        }
    }
    return max(K, best_B);  // never cap below K
}
"""

# Insert the function before main()
idx = code.find("int main()")
code = code[:idx] + optimal_fn + "\n" + code[idx:]

# Add a call to compute_optimal_B() after reading the task-time table
old_sort = """    d_proc_tab.sort_entries();
    d_post_tab.sort_entries();
    
    // Contiguous vector storage"""
new_sort = """    d_proc_tab.sort_entries();
    d_post_tab.sort_entries();
    
    int optimal_B_star = compute_optimal_B();
    
    // Contiguous vector storage"""
code = code.replace(old_sort, new_sort)

# Use optimal_B_star for D PRE cap
old_dpre = """            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                stringstream ss;
                ss << "E D PRE -1 " << ready_d_pre.size();
                for (int rid : ready_d_pre) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }"""
new_dpre = """            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                int batch = min((int)ready_d_pre.size(), optimal_B_star);
                stringstream ss;
                ss << "E D PRE -1 " << batch;
                for (int i = 0; i < batch; ++i) {
                    int rid = ready_d_pre[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }"""
code = code.replace(old_dpre, new_dpre)

# Also cap D POST at optimal_B_star
old_dpost = """            if (!ready_d_post.empty() && !hold_d_post) {
                stringstream ss;
                ss << "E D POST -1 " << ready_d_post.size();
                for (int rid : ready_d_post) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_POST;
                }"""
new_dpost = """            if (!ready_d_post.empty() && !hold_d_post) {
                int batch_dp = min((int)ready_d_post.size(), optimal_B_star);
                stringstream ss;
                ss << "E D POST -1 " << batch_dp;
                for (int i = 0; i < batch_dp; ++i) {
                    int rid = ready_d_post[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_POST;
                }"""
code = code.replace(old_dpost, new_dpost)

with open("test_optimal_B.cpp", "w") as f:
    f.write(code)

subprocess.run("g++ -std=c++17 -O2 -o test_optimal_B test_optimal_B.cpp", shell=True)
res = subprocess.run("python3 benchmark.py best_15630 test_optimal_B", shell=True, 
                     stdout=subprocess.PIPE, text=True)

# Extract key metrics
for line in res.stdout.split('\n'):
    if any(x in line for x in ["Total Sum", "Regression Sum", "Max Individual", "VERDICT", "Regressed"]):
        print(line.strip())

