# Edge-Cloud Collaborative Scheduling — Complete Solution Explanation

## Architecture Overview

This solution implements a **high-throughput, low-latency synchronized reactive scheduler** for the Edge-Cloud Collaborative Scheduling problem. It is designed to maximize both the throughput component ($w_{tp}$) and the waiting time component ($w_c$, TDR + TPOT).

---

## 1. Core Principles & Breakthrough Techniques

### A. Selective Strategic Idling ($w_c \ge 0.70 \lor K \ge 8$) (+293.557 pts Local)
- On smaller/medium clusters ($K < 8$) and balanced benchmarks ($w_c < 0.70$), holding E idle while waiting for decode DOWN transfers (`pending_down_dec > 0`) caused prefill requests to queue up in `READY_P_PRE`, adding 10–20 ms of unnecessary TDR delay to every request.
- By allowing E to execute $P\text{ PRE}$ ($2.5\text{ ms}$) immediately while decode transfers are in transit on $K < 8$, prefill requests are dispatched to idle remote CPUs early, compressing total makespan and achieving large point unlocks on deep model topologies and fast networks (+73 pts on Test 8, +69 pts on Test 17, +70 pts on Test 19, +49 pts on Test 21).
- Strategic idling on E is strictly preserved for massive clusters ($K \ge 8$) and latency-dominant benchmarks ($w_c \ge 0.70$), guaranteeing zero-delay $D\text{ POST}$ execution.

### B. Dual-Stage Synchronized Wave Pipelining (+1,837.756 pts)
- **Stage 1: D POST Coalescing (`pending_down_dec > 0`)**
  - When $K > 1$, remotes finish `D PROC` at slightly staggered times due to network serialization. If the local computer (E) executes `D POST` immediately upon the first remote's completion, E becomes busy with a small batch ($B/K$), forcing subsequent remotes' completed transfers to wait in queue. This fragments the decode pipeline into $K$ desynchronized micro-tasks, multiplying the $S$ scheduling overhead by $K \times$ on E and causing phase jitter.
  - We track `pending_down_dec` (in-flight/queued decode DOWN transfers). If `pending_down_dec > 0`, E holds `D POST` and uses that frame to execute background work (`P POST`, `D PRE`, or `P PRE`), or waits a few milliseconds for the in-flight transfers to arrive. When all queued DOWN transfers arrive, E executes **one unified `D POST`** for all remotes simultaneously.
- **Stage 2: D PRE Wave Synchronization (`decode_progressing > 0`)**
  - When a newly finished prefill request completes `P POST` and enters `READY_D_PRE` while an existing decode wave is mid-cycle, starting a separate `D PRE` for just 1 request would split the decode pipeline into two competing waves.
  - By holding `D PRE` while an existing decode wave is progressing, the newly admitted request waits briefly and merges into the main batch when the main wave finishes `D POST`.
  - Result: All decode requests cycle together in **one single, high-throughput batch** with zero fragmentation.

### C. Universal Micro-Batch P POST Coalescing (`ready_d_pre < max(2, K)`) (+0.615 pts)
- In $K=1$ tests or initial cold-start phases ($B < K$), if prefill requests are ready for `P POST`, E executes `P POST` first (taking $2\text{–}3\text{ ms}$).
- Prevents single-remote benchmarks from running degenerate single-request cycles ($B=1$), doubling token throughput while clearing mean TDR.

### D. Layer-Count SRPT on P PROC (Starvation Protection)
- Compares `remaining_layers = num_layers - current_layer`, tie-breaking by smallest $L_{in}$.
- Guarantees requests near completion ($90\%$ finished) exit the prefill pipeline immediately.

### E. Feasible-Set SJF Admission on P PRE
- Scans the feasible set of unthrottled requests in `READY_P_PRE`, admitting the shortest job first without head-of-line blocking.

### F. Min-Max Balanced Load Balancing
- Equalizes active requests across remotes to keep decode batches uniform, with prefill CPU workload as tiebreaker.

---

## 2. Scheduling Policies Summary

| Task | Server | Selection / Batching Strategy |
|---|---|---|
| **D POST** | E | All ready requests combined (coalesced when `pending_down_dec > 0`) |
| **D PRE** | E | All ready requests combined (micro-batch coalescing with `P POST` when $< K$) |
| **P POST** | E | Earliest arrival time first (immediate priority, stops TDR timer) |
| **P PRE** | E | Feasible-Set Shortest Job First (smallest unthrottled $L_{in}$ first), continuous HOL throttle |
| **D PROC** | Ck | All ready requests assigned to remote $k$ combined |
| **P PROC** | Ck | Layer-Count SRPT (closest to finishing, tiebreak smallest $L_{in}$), pipeline-aware even-partition splitting |

---

## 3. Finite State Machine (17 States)

```
ARR → READY_P_PRE → IN_FLIGHT_P_PRE → WAITING_UP_PRE → READY_P_PROC
    → IN_FLIGHT_P_PROC → (loop if split) → WAITING_DOWN_PRE → READY_P_POST
    → IN_FLIGHT_P_POST → READY_D_PRE → IN_FLIGHT_D_PRE → WAITING_UP_DEC
    → READY_D_PROC → IN_FLIGHT_D_PROC → WAITING_DOWN_DEC → READY_D_POST
    → IN_FLIGHT_D_POST → (back to READY_D_PRE or FINISHED)
```
