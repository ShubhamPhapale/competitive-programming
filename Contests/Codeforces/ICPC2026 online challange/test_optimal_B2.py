import subprocess, re

code = open("best_15630.cpp").read()

# Better optimal B* function
optimal_fn = """
int compute_optimal_B() {
    // Strategy: find B that maximizes throughput while keeping TPOT under SLO2
    // If w_c > 0 and SLO2 is finite, find largest B where T_cycle <= SLO2
    // Then among those, pick the one with highest throughput
    // If no such B exists (even B=1 exceeds SLO2), just maximize throughput
    
    int best_B = K;
    double best_throughput = 0;
    bool found_feasible = false;
    
    for (int B = 1; B <= min(500, 20 * K); ++B) {
        int remotes_used = min(B, K);
        int b_per_remote = (B + remotes_used - 1) / remotes_used;
        
        double t_e = 2.0 * S + d_pre_tab.lookup(B) + d_post_tab.lookup(B);
        double t_up = 0;
        for (int k = 0; k < remotes_used; ++k) {
            t_up += transfer_time(b_per_remote);
        }
        double t_remote = S + d_proc_tab.lookup(b_per_remote);
        double t_down = t_up;
        
        double t_cycle = max({t_e, t_up, t_remote, t_down});
        double throughput = (double)B / t_cycle;
        
        bool feasible = (t_cycle <= SLO2) || (w_c < 0.01);
        
        if (feasible) {
            if (!found_feasible || throughput > best_throughput) {
                best_throughput = throughput;
                best_B = B;
                found_feasible = true;
            }
        } else if (!found_feasible) {
            // No feasible B yet, maximize throughput anyway
            if (throughput > best_throughput) {
                best_throughput = throughput;
                best_B = B;
            }
        }
    }
    return max(K, best_B);
}
"""

idx = code.find("int main()")
code = code[:idx] + optimal_fn + "\n" + code[idx:]

old_sort = """    d_proc_tab.sort_entries();
    d_post_tab.sort_entries();
    
    // Contiguous vector storage"""
new_sort = """    d_proc_tab.sort_entries();
    d_post_tab.sort_entries();
    
    int optimal_B_star = compute_optimal_B();
    
    // Contiguous vector storage"""
code = code.replace(old_sort, new_sort)

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

with open("test_optimal_B2.cpp", "w") as f:
    f.write(code)

subprocess.run("g++ -std=c++17 -O2 -o test_optimal_B2 test_optimal_B2.cpp", shell=True)
res = subprocess.run("python3 benchmark.py best_15630 test_optimal_B2", shell=True, 
                     stdout=subprocess.PIPE, text=True)

for line in res.stdout.split('\n'):
    if any(x in line for x in ["Total Sum", "Regression Sum", "Max Individual", "VERDICT", "Regressed", "Record", "Trimmed"]):
        print(line.strip())

