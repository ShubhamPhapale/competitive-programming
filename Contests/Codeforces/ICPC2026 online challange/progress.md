# Edge-Cloud Collaborative Scheduling — Contest Progress & Submission History

## Current Standing & Best Confirmed Submission
- **Active Codeforces Best:** **`15,630.115` points** ([`best_15630.cpp`](file:///Users/shubhamsarjeraophapale/Competitive%20Programming/Contests/Codeforces/ICPC2026%20online%20challange/best_15630.cpp))
- **Status:** Baseline Active & 100% Preserved (Rolled back after Sub 21 regression)
- **Target Goal:** **`17,000+` points**

---

## Verified Submission Log

| # | Submission ID / File | Codeforces Score | Status | Key Mechanism / Changes Tested |
|---|---|---|---|---|
| 1 | `baseline_1.cpp` | 11,802.000 | Baseline | Initial naive round-robin baseline |
| 2 | `baseline_2.cpp` | 11,804.000 | Baseline | Minor transfer tuning |
| 3 | `baseline_3.cpp` | 12,239.206 | Working | First unified decode batching |
| 4 | `baseline_4.cpp` | 12,159.695 | **Regression** | Batch-size capping via static $B_{opt}$ (Reverted) |
| 5 | `best_12623.cpp` | 12,623.504 | Working | Feasible-set SJF prefill admission |
| 6 | `best_13239.cpp` | 13,239.204 | Working | Basic P PROC chunk splitting |
| 7 | `baseline_7.cpp` | 13,238.360 | **Regression** | `prefill_in_pipeline >= 2` throttle (Reverted) |
| 8 | `best_15076.cpp` | 15,076.960 | **Major Breakthrough** | Dual-Stage Synchronized Wave Pipelining (`pending_down_dec > 0` + `decode_progressing > 0`) |
| 9 | `best_15629.cpp` | 15,629.500 | **Major Breakthrough** | Pipeline-Aware Slicing + Min-Max CPU Balancing |
| 10 | `attempt_10.cpp` | 15,608.128 | **Regression** | Priority inversion & timeout (Reverted) |
| 11 | `attempt_11.cpp` | 15,627.433 | **Regression** | $k \times \text{latency}$ remote stagger (Reverted) |
| 12 | `attempt_12.cpp` | 15,629.500 | Tied | Dynamic batch sizing |
| 13 | `best_15630.cpp` | **15,630.115** | **Current Best** | Micro-Batch $P\text{ POST}$ Coalescing before $D\text{ PRE}$ |
| 14 | `attempt_14.cpp` | 15,566.547 | **Regression** | True Time-Based SRPT starved 90%-finished requests (Reverted) |
| 15 | `attempt_15.cpp` | 15,629.781 | **Regression** | Strategic idling blocked $P\text{ POST}$ (Reverted) |
| 16 | `attempt_16.cpp` | 15,630.115 | Tied | Universal Micro-Batch $P\text{ POST}$ Coalescing (`max(2, K)`) |
| 17 | `attempt_17.cpp` | 15,371.367 | **Regression** | Gating $P\text{ POST}$ via active decode window inflated TDR (Reverted) |
| 18 | `attempt_18.cpp` | 15,574.382 | **Regression** | Relaxing `idle_for_down` on $K < 8$ added decode jitter to $D\text{ POST}$ (Reverted to `best_15630.cpp`) |
| 19 | `attempt_19.cpp` | 15,586.101 | **Regression** | Link queueing blind spot in `decode_arrival_time` caused excessive $1\text{ layer} \times S$ chunk fragmentation (Reverted to `best_15630.cpp`) |
| 20 | `attempt_20.cpp` | 15,629.793 | **Regression** | Round-trip pacing conditioned on $K=2 \lor (bw \ge 80 \land L \ge 32)$ regressed slightly (-0.322 pts) due to subtle timing jitter (Reverted to `best_15630.cpp`) |
| 21 | `attempt_21.cpp` | 15,576.409 | **Regression** | Predictive MPC state tracking with residual tail losses failed zero-regression invariant (-53.706 pts) (Reverted to `best_15630.cpp`) |
| 22 | `attempt_22.cpp` | 15,612.980 | **Regression** | Hardcoded fast cluster threshold (`bw >= 15.0 && S <= 2.0`) hyper-fragmented short models ($L \le 8$) (-17.135 pts) (Reverted to `best_15630.cpp`) |

---

## 50-Test Generalized Robustness Benchmark Suite

| Suite Category | Tests | Key Focus / Topologies Covered |
|---|---|---|
| **Core Contest Spectrum** | Tests 1 – 22 | Exact Example 1, Pure Throughput, Pure Latency QoS, Single-Node ($K=1$), Deep Models ($L=64$), Network Bottlenecks, Synchronous Bursts. |
| **Boundary & Extreme Regimes** | Tests 23 – 35 | Extreme overhead ($S=10$), Extreme Latency ($50\text{ ms}$), Long Output ($L_{out}=512$), Long Input ($L_{in}=4096$), Large Asymmetric Clusters ($K=3, 5, 6, 7$). |
| **Pathological Arrivals & Heavy Tails** | Tests 36 – 50 | Pareto Heavy-Tail input lengths ($80/20$ rule), Periodic Pulse bursts, Pareto bursty arrival intervals, Knife-edge constraints. |
