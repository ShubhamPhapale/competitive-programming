# Deep Analysis: Edge-Cloud Collaborative Scheduling — Road to 17000+

## Table of Contents
1. [Scoring Formula Decomposition](#1-scoring-formula-decomposition)
2. [System Bottleneck Analysis](#2-system-bottleneck-analysis)
3. [Critical Bugs in Current Solution](#3-critical-bugs-in-current-solution)
4. [Optimization Roadmap (Ranked by Impact)](#4-optimization-roadmap)

---

## 1. Scoring Formula Decomposition

```
Score = 1000 × (w_tp × throughput_component + w_c × waiting_component)
```

### Throughput Component (weight: w_tp)
```
tp = Σ L_out[i] / (latest_finish - earliest_arrival)
throughput_component = clamp(tp; tp_base, tp_UB)
```
- We must maximize **tokens produced per millisecond**.
- This means: minimize total elapsed time, maximize parallelism.
- `tp_base` is the rate of a naive serial scheduler. `tp_UB` is the theoretical max.

### Waiting Component (weight: w_c)
```
tdr  = mean over all requests of (P POST completion − arrival)
tpot = mean of (e_{j+1} − e_j) over ALL consecutive token gaps, pooled
excess_tdr  = max(0, (tdr − SLO1)/SLO1)
excess_tpot = max(0, (tpot − SLO2)/SLO2)
dist = sqrt(excess_tdr² + excess_tpot²)
waiting_component = clamp(dist; dist_base, 0)   ← LOWER dist is BETTER
```

> [!IMPORTANT]
> **TPOT is pooled across ALL requests.** A single request with a huge gap between two
> tokens drags down the entire score. Every request must produce tokens at a steady cadence.

> [!IMPORTANT]
> **TDR counts from arrival to P POST completion**, not to first token. Getting requests
> through the input pipeline fast is critical regardless of throughput strategy.

---

## 2. System Bottleneck Analysis

There are **4 shared resources** that create bottlenecks:

| Resource | Tasks it handles | Notes |
|---|---|---|
| **E (Local)** | P PRE, P POST, D PRE, D POST | **THE** bottleneck — handles ALL requests |
| **UP link** | FIFO queue, one transfer at a time | Shared by all prefill and decode UP transfers |
| **DOWN link** | FIFO queue, one transfer at a time | Shared by all prefill and decode DOWN transfers |
| **Ck (Remote k)** | P PROC, D PROC | Independent per remote |

### Steady-State Decode Cycle Analysis

For B requests cycling through decode, assigned uniformly to K remotes:

```
One decode cycle produces B tokens. The cycle time is:

T_cycle = max(T_E, T_UP, T_remote, T_DOWN)

where:
  T_E      = 2S + d_pre(B) + d_post(B)          ← E does D PRE + D POST
  T_UP     = K × (latency + 8×(B/K)×bpt / (bw×10⁶))  ← K sequential transfers
  T_remote = S + d_proc(B/K)                     ← parallel across K remotes
  T_DOWN   = K × (latency + 8×(B/K)×bpt / (bw×10⁶))  ← K sequential transfers

Throughput = B / T_cycle   tokens/ms
```

> [!TIP]
> **The optimal batch size B* maximizes B / T_cycle.** This is a concrete optimization 
> problem we can solve using the task-time table. We are NOT doing this at all.

### Why This Matters

If `d_pre(B)` grows faster than linearly (superlinear), larger batches decrease
E-throughput. If it grows slower (sublinear), larger batches help. **We must check
the actual table values** to decide.

Similarly for transfers: UP time = `K × latency + 8B×bpt/(bw×10⁶)`. The `K × latency`
term is FIXED overhead per cycle regardless of B. So larger B amortizes this overhead
better. But larger B also means longer D PROC and D POST, increasing TPOT.

**TPOT in steady state = T_cycle** (each request gets one token per cycle). So:
- Larger B → higher throughput but higher TPOT
- Smaller B → lower TPOT but more overhead per token

The optimal B balances these based on w_tp vs w_c.

---

## 3. Critical Bugs in Current Solution

### Bug 1: `!up_busy` guard on P PRE (Line 243) — **HARMFUL**
```cpp
if (best_p_pre != -1 && !up_busy && active_reqs < K * 16) {
```
P PRE runs on **E**, not the UP link. The UP transfer happens AFTER P PRE finishes
(duration `S + d_pre_time`). By then the UP link may be free. This condition
**unnecessarily starves the pipeline** of new requests, hurting both TDR and throughput.

### Bug 2: `!up_busy || ready_d_pre.size() >= 8` guard on D PRE (Line 223) — **HARMFUL**
```cpp
if (!ready_d_pre.empty() && (!up_busy || ready_d_pre.size() >= 8)) {
```
Same logic: D PRE runs on E. The UP transfers happen after D PRE completes. Blocking
D PRE because UP is busy just delays everything. The threshold of 8 is arbitrary and
has no mathematical basis.

### Bug 3: `le = -1` hack in P PROC (Lines 314-316) — **WASTES REMOTE**
```cpp
if (down_transfers > 0 && le == num_layers) {
    if (ls < num_layers - 1) le = num_layers - 1;
    else le = -1;  // ← remote sits completely IDLE!
}
```
When `le = -1`, the remote does NOTHING. The P PROC DOWN transfer will simply queue
behind existing DOWN transfers — that's fine, it's FIFO. Leaving the remote idle is
pure waste. The DOWN transfer from P PROC (size = L_in × bytes_per_token) might be
large, but queuing it is still better than doing nothing.

### Bug 4: `up_transfers` / `down_transfers` tracking is wrong
D PRE with requests from multiple remotes creates **one UP transfer per remote**. We
increment `up_transfers` by `remotes.size()`. But when XDN UP DEC arrives, we only
decrement by 1 (line 177). Each XDN UP event corresponds to ONE transfer, so the
decrement is correct, but we need to verify the increment logic matches: each D PRE
TDN should add exactly the number of distinct remotes. This seems correct.

However, there's a subtle issue: the `up_transfers` counter conflates prefill UP
transfers (large, size = L_in × bpt) with decode UP transfers (small, size = 
batch_per_remote × bpt). The "UP is busy" signal doesn't distinguish between a
1-token decode transfer (tiny) and a 4096-token prefill transfer (massive). This
makes the guard conditions even more harmful.

### Bug 5: Task-time table is ONLY used for P PROC splitting
We store `p_proc_table` but completely ignore:
- `p_pre_table` — could inform P PRE scheduling decisions
- `p_post_table` — could inform P POST scheduling decisions  
- **`d_pre_table`** — CRITICAL for optimal decode batch sizing
- **`d_proc_table`** — CRITICAL for optimal decode batch sizing
- **`d_post_table`** — CRITICAL for optimal decode batch sizing

This is arguably the single biggest missed optimization.

### Bug 6: `active_reqs < K * 16` admission control is arbitrary
The constant 16 has no mathematical basis. The optimal number of concurrent decode
requests depends on the specific system parameters (S, latency, bw, task times).

---

## 4. Optimization Roadmap (Ranked by Expected Impact)

### 🔴 Priority 1: Use the Task-Time Table for Optimal Batch Sizing

**Expected impact: MASSIVE (potentially +2000-3000 points)**

Currently we blindly batch ALL ready requests into one D PRE / D PROC / D POST.
This is catastrophically wrong in many scenarios.

**Implementation:**
1. Parse ALL 6 task-time columns into separate interpolation tables
2. Compute the optimal decode batch size B* that maximizes throughput:
   ```
   for B = 1 to max_possible:
       T_E = 2S + d_pre(B) + d_post(B)
       T_UP = K × (latency + 8×ceil(B/K)×bpt/(bw×1e6))
       T_remote = S + d_proc(ceil(B/K))
       T_DOWN = T_UP  (same formula)
       T_cycle = max(T_E, T_UP, T_remote, T_DOWN)
       throughput = B / T_cycle
   ```
3. When w_c > 0, also consider TPOT:
   - TPOT ≈ T_cycle in steady state
   - Penalize batch sizes where T_cycle > SLO2
4. Cap D PRE / D POST group sizes at B*
5. Cap D PROC group sizes at ceil(B*/K) per remote

### 🔴 Priority 2: Remove All Harmful Guards

**Expected impact: +500-1000 points (prevents pipeline stalls)**

Remove:
- `!up_busy` condition on P PRE
- `!up_busy || size >= 8` condition on D PRE
- `le = -1` P PROC hack
- `active_reqs < K * 16` arbitrary cap

Replace with principled admission control (see Priority 5).

### 🔴 Priority 3: Predictive Scheduling via Time Simulation

**Expected impact: +1000-2000 points**

We can **predict exactly when every transfer and task completes**:
- Transfer time = `latency + 8 × data_bytes / (bw × 10⁶)`
- Task duration = table lookup (we get `dur` in TDN, and can predict from table)

**Implementation:**
1. Track exact timestamps of: current time, when E becomes free, when each Ck 
   becomes free, when each UP/DOWN transfer completes
2. When E is free and both D POST and P POST are not ready, compute:
   - `t_d_post_ready` = when the next DOWN DEC transfer completes
   - `t_p_post_ready` = when the next DOWN PRE transfer completes
   - `duration_p_pre` = S + lookup(p_pre_table, L_in)
3. Decision: if `t_d_post_ready < current_time + duration_p_pre`, then **WAIT**
   for D POST instead of starting P PRE (because P PRE would block E past the
   D POST opportunity, delaying token production)
4. Otherwise, start P PRE (utilize E during the gap)

This is the single most sophisticated optimization and likely separates top solutions
from the rest.

### 🟡 Priority 4: Adaptive P PROC Splitting

**Expected impact: +200-500 points**

Current splitting uses `target_dur = max(2*S, 20.0)` which is arbitrary.

**Better approach:**
- If there are active decode requests on this remote (WAITING_UP_DEC or READY_D_PROC),
  split aggressively: target_dur = S + d_proc(expected_batch) to create exactly the
  right gap for one D PROC execution
- If NO decode requests are on this remote, process P PROC fully (no splitting needed)
- If `num_layers == 1`, don't split at all (impossible anyway)

### 🟡 Priority 5: Score-Weight-Aware Strategy

**Expected impact: +300-800 points**

The scoring weights w_tp and w_c fundamentally change the optimal strategy:

**When w_tp dominates (w_tp >> w_c):**
- Maximize throughput: use larger batches, pipeline aggressively
- Admit many requests quickly, process them in bulk
- P PROC splitting less important (just get requests through fast)

**When w_c dominates (w_c >> w_tp):**
- Minimize TDR: rush each request through input phase ASAP
- Minimize TPOT: keep decode batch sizes small for steady token cadence
- Avoid large batches that create long cycle times
- Optimal B* should satisfy: T_cycle(B*) ≤ SLO2

**When both matter equally:**
- Find the Pareto-optimal batch size
- Balance throughput against latency

### 🟡 Priority 6: Smart Admission Control

**Expected impact: +200-500 points**

Instead of arbitrary `K * 16`, compute the optimal number of concurrent decode
requests based on the throughput analysis:
- If increasing B beyond current count doesn't improve throughput (because E or 
  network is already the bottleneck), stop admitting
- If the system is remote-bottlenecked, admit more requests to different remotes
- Track how many requests are in each phase and maintain balance

### 🟢 Priority 7: Better Load Balancing

**Expected impact: +100-300 points**

Current load metric uses `L_in × (remaining layers / num_layers) + 50` which is
crude. Better:
- Estimate actual remaining time: use task-time table to compute remaining P PROC
  time + number of remaining D PROC iterations × d_proc time
- Account for queued UP/DOWN transfers that will delay this remote
- Consider which remote has the earliest expected free time

### 🟢 Priority 8: Transfer-Aware D PRE Composition

**Expected impact: +100-300 points**

When building a D PRE batch, each distinct remote creates a separate UP transfer.
With K=8 remotes and one request per remote, D PRE creates 8 UP transfers, each
with the full latency overhead.

If we split into 2 D PRE tasks (one for C0-C3, one for C4-C7), we get:
- 2 × S cost on E (worse)
- But each D PRE creates only 4 UP transfers
- AND the second batch's transfers can start while the first batch's D PROC runs

This might improve pipeline utilization when K is large.

### 🟢 Priority 9: Continuous Batching (Orca-style)

**Expected impact: +100-500 points**

Instead of waiting for ALL decode requests to reach the same state, process them
in waves:
- If 5 out of 10 requests are READY_D_POST while the other 5 are still in D PROC,
  do D POST for the 5 ready ones immediately
- This produces tokens sooner (better TPOT)
- Then do D PRE for those 5 while the other 5 are still processing

Our solution already does this to some extent, but the priority ordering and
grouping logic may prevent optimal interleaving.

---

## Summary: Implementation Order

| # | Optimization | Expected Δ | Complexity |
|---|---|---|---|
| 1 | Parse all 6 task-time tables + compute optimal batch size | +2000-3000 | Medium |
| 2 | Remove harmful guards (up_busy, le=-1, etc.) | +500-1000 | Easy |
| 3 | Predictive scheduling (time simulation) | +1000-2000 | Hard |
| 4 | Adaptive P PROC splitting based on decode load | +200-500 | Medium |
| 5 | Score-weight-aware batch sizing (w_tp vs w_c) | +300-800 | Medium |
| 6 | Smart admission control | +200-500 | Medium |
| 7 | Better load balancing with time estimates | +100-300 | Medium |
| 8 | Transfer-aware D PRE composition | +100-300 | Hard |
| 9 | Continuous batching | +100-500 | Hard |

**Combined theoretical maximum improvement: ~5000-8000 points → target: 17000-20000**

> [!CAUTION]
> Priorities 1 and 2 alone could yield +3000 points. Implement these first and test
> before moving to the harder predictive scheduling.
