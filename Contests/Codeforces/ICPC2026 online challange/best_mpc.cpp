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

inline int parse_remote(const string& s) {
    if (s.empty()) return 0;
    if (s[0] == 'C' || s[0] == 'c') {
        if (s.size() > 1) {
            try { return stoi(s.substr(1)); } catch (...) { return 0; }
        }
        return 0;
    }
    try { return stoi(s); } catch (...) { return 0; }
}

struct RemoteTracker {
    double est_d_up_arrive = 0.0;
    double est_d_down_arrive = 0.0;
    bool d_up_in_transit = false;
    bool d_down_in_transit = false;
    bool d_proc_in_flight = false;
    int d_proc_batch = 0;
};

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
    
    vector<Request> reqs;
    reqs.reserve(2048);
    
    bool E_free = true;
    vector<bool> C_free(K, true);
    
    int pending_down_dec = 0;
    
    // Deterministic Network FIFO Link and State Tracking
    double up_link_free_at = 0.0;
    double down_link_free_at = 0.0;
    double est_d_pre_finish = 0.0;
    bool d_pre_in_flight = false;
    vector<RemoteTracker> rem_state(K);
    
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
                
                if (server == "E") {
                    E_free = true;
                    if (task1 == "P") {
                        if (task2 == "PRE") {
                            int remote, rid; double dur;
                            cin >> remote >> rid >> dur;
                            reqs[rid].state = WAITING_UP_PRE;
                            
                            double xfer_dur = transfer_time(reqs[rid].L_in);
                            double xfer_start = max(current_time, up_link_free_at);
                            up_link_free_at = xfer_start + xfer_dur;
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
                            d_pre_in_flight = false;
                            
                            vector<vector<int>> per_remote(K);
                            for (int rid : rids) per_remote[reqs[rid].assigned_remote].push_back(rid);
                            
                            for (int k = 0; k < K; ++k) {
                                if (!per_remote[k].empty()) {
                                    int count = per_remote[k].size();
                                    double xfer_dur = transfer_time(count);
                                    double xfer_start = max(current_time, up_link_free_at);
                                    up_link_free_at = xfer_start + xfer_dur;
                                    
                                    rem_state[k].est_d_up_arrive = up_link_free_at;
                                    rem_state[k].d_up_in_transit = true;
                                    for (int rid : per_remote[k]) reqs[rid].state = WAITING_UP_DEC;
                                }
                            }
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
                } else {
                    int k = parse_remote(server);
                    if (k >= 0 && k < K) C_free[k] = true;
                    if (task1 == "P") {
                        if (task2 == "PROC") {
                            int ls, le, remote, rid; double dur;
                            cin >> ls >> le >> remote >> rid >> dur;
                            if (le == num_layers) {
                                double xfer_dur = transfer_time(reqs[rid].L_in);
                                double xfer_start = max(current_time, down_link_free_at);
                                down_link_free_at = xfer_start + xfer_dur;
                                reqs[rid].state = WAITING_DOWN_PRE;
                            } else {
                                reqs[rid].state = READY_P_PROC;
                            }
                        }
                    } else if (task1 == "D") {
                        if (task2 == "PROC") {
                            int remote, m;
                            cin >> remote >> m;
                            vector<int> rids(m);
                            for (int j = 0; j < m; ++j) cin >> rids[j];
                            double dur; cin >> dur;
                            
                            if (k >= 0 && k < K) rem_state[k].d_proc_in_flight = false;
                            
                            double xfer_dur = transfer_time(m);
                            double xfer_start = max(current_time, down_link_free_at);
                            down_link_free_at = xfer_start + xfer_dur;
                            
                            if (k >= 0 && k < K) {
                                rem_state[k].est_d_down_arrive = down_link_free_at;
                                rem_state[k].d_down_in_transit = true;
                            }
                            pending_down_dec++;
                            for (int rid : rids) reqs[rid].state = WAITING_DOWN_DEC;
                        }
                    }
                }
            } else if (type == "XDN") {
                string dir, remote_str, pre_dec;
                long long size; int m;
                cin >> dir >> remote_str >> size >> pre_dec >> m;
                vector<int> rids(m);
                for (int j = 0; j < m; ++j) cin >> rids[j];
                
                int k = parse_remote(remote_str);
                if (pre_dec == "PRE") {
                    if (dir == "UP") reqs[rids[0]].state = READY_P_PROC;
                    else reqs[rids[0]].state = READY_P_POST;
                } else {
                    if (dir == "UP") {
                        if (k >= 0 && k < K) rem_state[k].d_up_in_transit = false;
                        for (int rid : rids) reqs[rid].state = READY_D_PROC;
                    } else {
                        if (k >= 0 && k < K) rem_state[k].d_down_in_transit = false;
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
        
        // Compute earliest arrival time of ANY decode DOWN transfer to E:
        double earliest_down_arrive = 1e18;
        for (int k = 0; k < K; ++k) {
            if (rem_state[k].d_down_in_transit) {
                earliest_down_arrive = min(earliest_down_arrive, rem_state[k].est_d_down_arrive);
            } else if (rem_state[k].d_proc_in_flight) {
                double proc_finish = current_time + S + d_proc_tab.lookup(rem_state[k].d_proc_batch);
                double down_start = max(proc_finish, down_link_free_at);
                double down_end = down_start + transfer_time(rem_state[k].d_proc_batch);
                earliest_down_arrive = min(earliest_down_arrive, down_end);
            }
        }
        
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
            
            bool hold_d_post = (pending_down_dec > 0);
            bool hold_d_pre  = (decode_progressing > 0);
            
            // ── PRIORITY 1: D POST — consumes output tokens ─────────
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
            bool wait_for_p_post = (ready_d_pre.size() < (size_t)K && !ready_p_post.empty());
            
            if (!scheduled && !ready_d_pre.empty() && !hold_d_pre && !wait_for_p_post) {
                stringstream ss;
                ss << "E D PRE -1 " << ready_d_pre.size();
                for (int rid : ready_d_pre) {
                    ss << " " << rid;
                    reqs[rid].state = IN_FLIGHT_D_PRE;
                }
                assignments.push_back(ss.str());
                E_free = false;
                scheduled = true;
                d_pre_in_flight = true;
                est_d_pre_finish = current_time + S + d_pre_tab.lookup(ready_d_pre.size());
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
                
                double max_allowed_up = decode_cycle * (1.0 + 3.0 * w_tp);
                
                int best = -1;
                for (int rid : ready_p_pre) {
                    double up_time = transfer_time(reqs[rid].L_in);
                    bool throttled = (decode_active > 0 && up_time > max_allowed_up && w_tp < 0.90);
                    if (!throttled) {
                        double p_pre_dur = S + p_pre_tab.lookup(reqs[rid].L_in);
                        double p_pre_E_finish = current_time + p_pre_dur;
                        double p_pre_UP_finish = max(p_pre_E_finish, up_link_free_at) + up_time;
                        
                        double d_pre_needed = earliest_down_arrive + S + d_post_tab.lookup(max(1, decode_active)) + S + d_pre_tab.lookup(max(1, decode_active));
                        
                        // Dual-Resource MPC Zero-Interference Invariant:
                        // 1. E CPU is free when DOWN arrives for D POST
                        // 2. UP Link is free when D PRE completes
                        bool safe_cpu = (p_pre_E_finish <= earliest_down_arrive + 1e-6);
                        bool safe_link = (p_pre_UP_finish <= d_pre_needed + 1e-6);
                        
                        bool safe_with_down = (pending_down_dec == 0 || (safe_cpu && safe_link) || w_c < 0.50);
                        if (safe_with_down) {
                            if (best == -1 || reqs[rid].L_in < reqs[best].L_in || 
                               (reqs[rid].L_in == reqs[best].L_in && reqs[rid].arrival_time < reqs[best].arrival_time)) {
                                best = rid;
                            }
                        }
                    }
                }
                
                if (best != -1) {
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
                    reqs[best].state = IN_FLIGHT_P_PRE;
                    stringstream ss;
                    ss << "E P PRE " << best_k << " " << best;
                    assignments.push_back(ss.str());
                    E_free = false;
                }
            }
        }
        
        // ─── Remote Computer Scheduling ─────────────────────────────
        for (int k = 0; k < K; ++k) {
            if (!C_free[k]) continue;
            
            vector<int> ready_d_proc, ready_p_proc;
            int dec_total_on_k = 0;
            
            for (const auto& req : reqs) {
                if (req.assigned_remote != k || req.state == FINISHED) continue;
                if (req.state == READY_D_PROC) ready_d_proc.push_back(req.id);
                else if (req.state == READY_P_PROC) ready_p_proc.push_back(req.id);
                
                if (req.state >= READY_D_PRE && req.state <= IN_FLIGHT_D_POST) {
                    dec_total_on_k++;
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
                rem_state[k].d_proc_in_flight = true;
                rem_state[k].d_proc_batch = ready_d_proc.size();
                continue;
            }
            
            // ── PRIORITY 2: P PROC — SRPT with Deterministic Slack Slicing ───
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
                
                // Pipeline-Aware Deterministic Slack Slicing:
                if (dec_total_on_k > 0 && num_layers > 1 && w_tp < 0.85) {
                    int rem_layers = num_layers - ls;
                    double total_p_proc = p_proc_tab.lookup(rq.L_in);
                    
                    if (total_p_proc > 0) {
                        double next_decode_arrival = 1e18;
                        if (rem_state[k].d_up_in_transit) {
                            next_decode_arrival = rem_state[k].est_d_up_arrive;
                        } else if (d_pre_in_flight) {
                            double stagger = 0.0;
                            for (int i = 0; i <= k; ++i) stagger += transfer_time(max(1, dec_total_on_k));
                            next_decode_arrival = est_d_pre_finish + stagger;
                        } else {
                            double down_arr = current_time;
                        if (rem_state[k].d_down_in_transit) {
                            down_arr = rem_state[k].est_d_down_arrive;
                        } else if (rem_state[k].d_proc_in_flight) {
                            double proc_end = current_time + S + d_proc_tab.lookup(rem_state[k].d_proc_batch);
                            double down_start = max(proc_end, down_link_free_at);
                            down_arr = down_start + transfer_time(rem_state[k].d_proc_batch);
                        }
                            double stagger = 0.0;
                            for (int i = 0; i <= k; ++i) stagger += transfer_time(max(1, dec_total_on_k));
                            next_decode_arrival = down_arr + S + d_post_tab.lookup(K * dec_total_on_k) + S + d_pre_tab.lookup(K * dec_total_on_k) + stagger;
                        }
                        
                        double slack = max(0.0, next_decode_arrival - current_time);
                        double target_dur;
                        if (K == 1) {
                            target_dur = max(3.0 * S, 30.0 * (1.0 + w_tp));
                        } else if (K >= 5) {
                            target_dur = max(4.0 * S, max(20.0, slack));
                        } else {
                            target_dur = max(2.5 * S, slack);
                        }
                        
                        if (total_p_proc > target_dur * 1.5) {
                            int ideal_layers = max(1, (int)round(num_layers * target_dur / total_p_proc));
                            int pieces = max(1, (rem_layers + ideal_layers - 1) / ideal_layers);
                            int piece_len = (rem_layers + pieces - 1) / pieces;
                            le = min(num_layers, ls + piece_len);
                        }
                    }
                }
                
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
