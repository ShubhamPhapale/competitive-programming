#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

enum State {
    READY_P_PRE = 0,
    IN_FLIGHT_P_PRE = 1,
    WAITING_UP_PRE = 2,
    READY_P_PROC = 3,
    IN_FLIGHT_P_PROC = 4,
    WAITING_DOWN_PRE = 5,
    READY_P_POST = 6,
    IN_FLIGHT_P_POST = 7,
    READY_D_PRE = 8,
    IN_FLIGHT_D_PRE = 9,
    WAITING_UP_DEC = 10,
    READY_D_PROC = 11,
    IN_FLIGHT_D_PROC = 12,
    WAITING_DOWN_DEC = 13,
    READY_D_POST = 14,
    IN_FLIGHT_D_POST = 15,
    FINISHED = 16
};

struct Request {
    int id;
    int L_in;
    int assigned_remote;
    State state;
    int current_layer;
    double arrival_time;
};

// ─── Task-Time Interpolation Tables ─────────────────────────────────
struct TimeTable {
    vector<pair<int,double>> entries; // sorted by batch_size
    
    void add(int bs, double val) {
        if (val >= 0) entries.push_back({bs, val});
    }
    void sort_entries() {
        sort(entries.begin(), entries.end());
    }
    double lookup(int size) const {
        if (entries.empty()) return 1.0;
        if (size <= entries.front().first) return entries.front().second;
        if (size >= entries.back().first) return entries.back().second;
        for (size_t i = 0; i < entries.size() - 1; ++i) {
            if (size >= entries[i].first && size <= entries[i+1].first) {
                double x0 = entries[i].first, y0 = entries[i].second;
                double x1 = entries[i+1].first, y1 = entries[i+1].second;
                if (x0 == x1) return y0;
                return y0 + (y1 - y0) * (size - x0) / (x1 - x0);
            }
        }
        return entries.back().second;
    }
};

TimeTable p_pre_tab, p_proc_tab, p_post_tab, d_pre_tab, d_proc_tab, d_post_tab;

// ─── System Parameters ──────────────────────────────────────────────
int K, bytes_per_token, num_layers;
double S, latency, bw;
double SLO1, SLO2, tp_UB, tp_base, dist_base, w_tp, w_c;

double transfer_time(int len) {
    return latency + 8.0 * (double)len * (double)bytes_per_token / (bw * 1e6);
}

int main() {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (!(cin >> K >> S >> latency >> bw >> bytes_per_token >> num_layers)) return 0;
    cin >> SLO1 >> SLO2 >> tp_UB >> tp_base >> dist_base >> w_tp >> w_c;
    
    int N;
    cin >> N;
    for (int i = 0; i < N; ++i) {
        int batch;
        double ppre, pproc, ppost, dpre, dproc, dpost;
        cin >> batch >> ppre >> pproc >> ppost >> dpre >> dproc >> dpost;
        p_pre_tab.add(batch, ppre);
        p_proc_tab.add(batch, pproc);
        p_post_tab.add(batch, ppost);
        d_pre_tab.add(batch, dpre);
        d_proc_tab.add(batch, dproc);
        d_post_tab.add(batch, dpost);
    }
    p_pre_tab.sort_entries();
    p_proc_tab.sort_entries();
    p_post_tab.sort_entries();
    d_pre_tab.sort_entries();
    d_proc_tab.sort_entries();
    d_post_tab.sort_entries();
    
    // Contiguous vector storage for O(1) request access and cache locality
    vector<Request> reqs;
    reqs.reserve(2048);
    
    bool E_free = true;
    vector<bool> C_free(K, true);
    
    // Track in-flight decode DOWN transfers for synchronized wave coalescing
    int pending_down_dec = 0;
    
    double current_time = 0;
    
    string t_str;
    while (cin >> t_str) {
        if (t_str == "END") break;
        
        current_time = stod(t_str);
        
        int e;
        cin >> e;
        for (int i = 0; i < e; ++i) {
            string type;
            cin >> type;
            if (type == "ARR") {
                int rid, Lin;
                cin >> rid >> Lin;
                if (rid >= (int)reqs.size()) reqs.resize(rid + 1);
                reqs[rid] = {rid, Lin, -1, READY_P_PRE, 0, current_time};
            } else if (type == "TDN") {
                string server, task1, task2;
                cin >> server >> task1 >> task2;
                
                if (server == "E") E_free = true;
                else {
                    int k = stoi(server.substr(1));
                    C_free[k] = true;
                }
                
                if (task1 == "P") {
                    if (task2 == "PRE") {
                        int remote, rid; double dur;
                        cin >> remote >> rid >> dur;
                        reqs[rid].state = WAITING_UP_PRE;
                    } else if (task2 == "PROC") {
                        int ls, le, remote, rid; double dur;
                        cin >> ls >> le >> remote >> rid >> dur;
                        if (le == num_layers) {
                            reqs[rid].state = WAITING_DOWN_PRE;
                        } else {
                            reqs[rid].state = READY_P_PROC;
                        }
                    } else if (task2 == "POST") {
                        int remote, rid; double dur;
                        cin >> remote >> rid >> dur;
                        reqs[rid].state = READY_D_PRE;
                    }
                } else if (task1 == "D") {
                    if (task2 == "PRE") {
                        int minus1, m;
                        cin >> minus1 >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) reqs[rid].state = WAITING_UP_DEC;
                    } else if (task2 == "PROC") {
                        int remote, m;
                        cin >> remote >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;
                        pending_down_dec++;
                    } else if (task2 == "POST") {
                        int minus1, m;
                        cin >> minus1 >> m;
                        vector<int> rids(m);
                        for (int j = 0; j < m; ++j) cin >> rids[j];
                        double dur; cin >> dur;
                        for (int rid : rids) {
                            if (reqs[rid].state != FINISHED) reqs[rid].state = READY_D_PRE;
                        }
                    }
                }
            } else if (type == "XDN") {
                string dir, remote_str, pre_dec;
                long long size; int m;
                cin >> dir >> remote_str >> size >> pre_dec >> m;
                vector<int> rids(m);
                for (int j = 0; j < m; ++j) cin >> rids[j];
                
                if (pre_dec == "PRE") {
                    if (dir == "UP") reqs[rids[0]].state = READY_P_PROC;
                    else reqs[rids[0]].state = READY_P_POST;
                } else {
                    if (dir == "UP") {
                        for (int rid : rids) reqs[rid].state = READY_D_PROC;
                    } else {
                        for (int rid : rids) reqs[rid].state = READY_D_POST;
                        pending_down_dec = max(0, pending_down_dec - 1);
                    }
                }
            } else if (type == "FIN") {
                int rid;
                cin >> rid;
                reqs[rid].state = FINISHED;
            }
        }
        
        // ═══════════════════════════════════════════════════════════════
        //  SCHEDULING DECISIONS
        // ═══════════════════════════════════════════════════════════════
        
        vector<string> assignments;
        
        // ─── E (Local Computer) Scheduling ──────────────────────────
        if (E_free) {
            vector<int> ready_d_post, ready_d_pre, ready_p_post, ready_p_pre;
            int decode_active = 0;
            int decode_progressing = 0;
            
            for (const auto& req : reqs) {
                if (req.state == FINISHED) continue;
                switch (req.state) {
                    case READY_D_POST: ready_d_post.push_back(req.id); break;
                    case READY_D_PRE:  ready_d_pre.push_back(req.id);  break;
                    case READY_P_POST: ready_p_post.push_back(req.id); break;
                    case READY_P_PRE:  ready_p_pre.push_back(req.id);  break;
                    default: break;
                }
                if (req.state >= READY_D_PRE && req.state <= IN_FLIGHT_D_POST) decode_active++;
                if (req.state == IN_FLIGHT_D_PRE || 
                    req.state == WAITING_UP_DEC || 
                    req.state == IN_FLIGHT_D_PROC || 
                    req.state == WAITING_DOWN_DEC || 
                    req.state == READY_D_POST || 
                    req.state == IN_FLIGHT_D_POST) {
                    decode_progressing++;
                } else if (req.state == READY_D_PROC && C_free[req.assigned_remote]) {
                    decode_progressing++;
                }
            }
            
            bool scheduled = false;
            
            // Synchronized Wave Coalescing guards:
            bool hold_d_post = (pending_down_dec > 0);
            bool hold_d_pre  = (decode_progressing > 0);
            
            // ── PRIORITY 1: D POST — produces tokens immediately ────
            if (!ready_d_post.empty() && !hold_d_post) {
                stringstream ss;
                ss << "E D POST -1 " << ready_d_post.size();
                for (int rid : ready_d_post) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_POST;
                }
                assignments.push_back(ss.str());
                E_free = false;
                scheduled = true;
            }
            
            // ── PRIORITY 2: D PRE — starts next decode wave ─────────
            // If ready decode count is smaller than K and P POST is ready,
            // prioritize P POST to admit the request into this decode wave and clear TDR.
            bool wait_for_p_post = (ready_d_pre.size() < (size_t)K && !ready_p_post.empty());
            
            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                int cap = 80;
                int batch = min((int)ready_d_pre.size(), max(1, cap));
                stringstream ss;
                ss << "E D PRE -1 " << batch;
                for (int i = 0; i < batch; ++i) {
                    int rid = ready_d_pre[i];
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }
                assignments.push_back(ss.str());
                E_free = false;
                scheduled = true;
            }
            
            // ── PRIORITY 3: P POST — completes input phase (TDR) ────
            if (!scheduled && !ready_p_post.empty()) {
                int best = ready_p_post[0];
                for (int rid : ready_p_post) {
                    if (reqs[rid].arrival_time < reqs[best].arrival_time) best = rid;
                }
                stringstream ss;
                ss << "E P POST " << reqs[best].assigned_remote << " " << best;
                reqs[best].state = IN_FLIGHT_P_POST;
                assignments.push_back(ss.str());
                E_free = false;
                scheduled = true;
            }
            
            // ── LOWEST PRIORITY: P PRE — admits new request ─────────
            if (!scheduled && !ready_p_pre.empty()) {
                double decode_cycle = 0;
                if (decode_active > 0) {
                    int per_remote = max(1, (decode_active + K - 1) / K);
                    decode_cycle = 2*S + d_pre_tab.lookup(decode_active) + d_post_tab.lookup(decode_active)
                                 + S + d_proc_tab.lookup(per_remote)
                                 + 2.0 * K * transfer_time(per_remote);
                }
                
                // Continuous Weight-Aware HOL Throttle threshold:
                double max_allowed_up = decode_cycle * (1.0 + 3.0 * w_tp);
                
                // Strategic Idling on E: If decode DOWN transfers are in transit and latency matters
                bool idle_for_down = (pending_down_dec > 0 && w_c >= 0.50);
                
                if (!idle_for_down) {
                    int best = -1;
                    for (int rid : ready_p_pre) {
                        double up_time = transfer_time(reqs[rid].L_in);
                        bool throttled = (decode_active > 0 && up_time > max_allowed_up && w_tp < 0.90);
                        if (!throttled) {
                            if (best == -1 || reqs[rid].L_in < reqs[best].L_in || 
                               (reqs[rid].L_in == reqs[best].L_in && reqs[rid].arrival_time < reqs[best].arrival_time)) {
                                best = rid;
                            }
                        }
                    }
                    
                    if (best != -1) {
                        // Min-Max Balanced Load Balancing:
                        // Primary: equalize active requests across remotes to keep decode batches uniform
                        // Secondary: minimize remaining prefill computation
                        int best_k = 0;
                        int min_active = 1e9;
                        double min_prefill_load = 1e18;
                        
                        for (int k = 0; k < K; ++k) {
                            int active_on_k = 0;
                            double prefill_load = 0;
                            for (const auto& rq : reqs) {
                                if (rq.assigned_remote == k && rq.state != FINISHED && rq.state != READY_P_PRE) {
                                    active_on_k++;
                                    if (rq.state <= WAITING_DOWN_PRE) {
                                        double pp = p_proc_tab.lookup(rq.L_in);
                                        prefill_load += pp * (double)(num_layers - rq.current_layer) / num_layers;
                                    }
                                }
                            }
                            if (active_on_k < min_active || (active_on_k == min_active && prefill_load < min_prefill_load)) {
                                min_active = active_on_k;
                                min_prefill_load = prefill_load;
                                best_k = k;
                            }
                        }
                        reqs[best].assigned_remote = best_k;
                        
                        stringstream ss;
                        ss << "E P PRE " << best_k << " " << best;
                        reqs[best].state = IN_FLIGHT_P_PRE;
                        assignments.push_back(ss.str());
                        E_free = false;
                        scheduled = true;
                    }
                }
            }
        }
        
        // ─── Remote Computer Scheduling ─────────────────────────────
        for (int k = 0; k < K; ++k) {
            if (!C_free[k]) continue;
            
            vector<int> ready_d_proc, ready_p_proc;
            int dec_waiting_up = 0;
            int dec_in_dpre = 0;
            int dec_total_on_k = 0;
            
            for (const auto& req : reqs) {
                if (req.assigned_remote != k || req.state == FINISHED) continue;
                if (req.state == READY_D_PROC) ready_d_proc.push_back(req.id);
                else if (req.state == READY_P_PROC) ready_p_proc.push_back(req.id);
                
                if (req.state >= READY_D_PRE && req.state <= IN_FLIGHT_D_POST) {
                    dec_total_on_k++;
                    if (req.state == WAITING_UP_DEC) dec_waiting_up++;
                    if (req.state == IN_FLIGHT_D_PRE) dec_in_dpre++;
                }
            }
            
            // ── PRIORITY 1: D PROC — ALL ready on this remote ──────
            if (!ready_d_proc.empty()) {
                stringstream ss;
                ss << "C" << k << " D PROC " << k << " " << ready_d_proc.size();
                for (int rid : ready_d_proc) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PROC;
                }
                assignments.push_back(ss.str());
                C_free[k] = false;
                continue;
            }
            
            // ── PRIORITY 2: P PROC — SRPT (closest to finishing) ───
            if (!ready_p_proc.empty()) {
                int best = ready_p_proc[0];
                for (int rid : ready_p_proc) {
                    int rem_r = num_layers - reqs[rid].current_layer;
                    int rem_b = num_layers - reqs[best].current_layer;
                    if (rem_r < rem_b || (rem_r == rem_b && reqs[rid].L_in < reqs[best].L_in))
                        best = rid;
                }
                
                Request& rq = reqs[best];
                int ls = rq.current_layer;
                int le = num_layers;
                
                // Pipeline-Aware Even-Partition Splitting:
                // Only split if decode work exists on remote k AND throughput is not pure dominant
                if (dec_total_on_k > 0 && num_layers > 1 && w_tp < 0.85) {
                    int rem_layers = num_layers - ls;
                    double total_p_proc = p_proc_tab.lookup(rq.L_in);
                    
                    if (total_p_proc > 0) {
                        double target_dur;
                        if (dec_waiting_up > 0) {
                            // Decode UP transfer is already in flight (imminent in 1-5ms)
                            double d_time = d_proc_tab.lookup(1);
                            target_dur = max(S + d_time, S * (2.0 + 2.0 * w_tp));
                        } else if (dec_in_dpre > 0) {
                            // D PRE running on E (arriving in ~10-15ms)
                            target_dur = max(2.0 * S, 15.0 * (1.0 + w_tp));
                        } else {
                            // Decode is far away in pipeline (in D POST / D PRE)
                            target_dur = max(3.0 * S, 30.0 * (1.0 + w_tp));
                        }
                        
                        if (total_p_proc > target_dur * 1.5) {
                            int ideal_layers = max(1, (int)round(num_layers * target_dur / total_p_proc));
                            int pieces = max(1, (rem_layers + ideal_layers - 1) / ideal_layers);
                            int piece_len = (rem_layers + pieces - 1) / pieces;
                            le = min(num_layers, ls + piece_len);
                        }
                    }
                }
                // If dec_total_on_k == 0 or w_tp >= 0.85: run full piece [ls, num_layers) to save S
                
                stringstream ss;
                ss << "C" << k << " P PROC " << ls << " " << le << " " << k << " " << best;
                rq.state = IN_FLIGHT_P_PROC;
                rq.current_layer = le;
                assignments.push_back(ss.str());
                C_free[k] = false;
            }
        }
        
        cout << assignments.size() << "\n";
        for (const string& a : assignments) {
            cout << a << "\n";
        }
        cout << flush;
    }
    return 0;
}
