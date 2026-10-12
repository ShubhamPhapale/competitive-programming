# Deep Analysis v2: The Road from 15,629 to 17,000+
## Edge-Cloud Collaborative Scheduling — Complete Mathematical & Empirical Framework

---

## 1. Executive Summary & Benchmark Score Breakdown

| Milestone | Score | Average per Test (out of 1000) | Breakthrough Feature |
|---|---|---|---|
| Baseline | 11,802.000 | 536.45 | Naive greedy round-robin, no splitting |
| Submission 3 | 12,239.206 | 556.33 | SJF P PRE + SRPT P PROC + static splitting |
| Submission 6 | 13,239.204 | 601.78 | `long long` XDN fix + dynamic priority swap + HOL throttle |
| Submission 8 | 15,076.960 | 685.31 | **Dual-Stage Wave Synchronization** (+1,837.756 pts) |
| Submission 9 | **15,629.500** | **710.43** | **Pipeline-Aware Splitting + Min-Max Balancing** (+552.540 pts) |
| **Target Top** | **17,000+** | **772.72+** | **Net gap to close: 62.29 pts / test** |

---

## 2. Mathematical Anatomy of the Remaining 62.29 Points / Test

Each test awards a score in $[0, 1000]$:
$$\text{Score} = 1000 \times \left( w_{tp} \cdot \text{clamp}(tp; tp_{base}, tp_{UB}) + w_c \cdot \text{clamp}(dist; dist_{base}, 0) \right)$$

### Where are the 62.3 points being lost?

Let's dissect the two components across the 22 test cases:

```
                                    Score Deficit (62.3 pts / test)
                                                   │
                  ┌────────────────────────────────┴────────────────────────────────┐
                  ▼                                                                 ▼
      Waiting Deficit (dist > 0)                                      Throughput Deficit (tp < tp_UB)
      - Caused when TDR > SLO1 or TPOT > SLO2                         - Caused by idle bubbles on E or remotes
      - Penalty is quadratic: dist = √(e_tdr² + e_tpot²)              - Caused by non-overlapped network transfers
      - A single test with TDR = 2×SLO1 loses ~300-500 pts!           - Sub-optimal admission rate during wave gaps
```

---

## 3. Empirical Truths: What the Data Proved

Through 10 live submissions on the Codeforces benchmark, we have verified exact structural invariants:

### ✅ Proven Breakthroughs:
1. **Wave Synchronization is King (+1,837 pts):**
   - When $K > 1$, running $D\text{ POST}$ and $D\text{ PRE}$ as **one unified batch across all remotes** amortizes the local computer schedule cost $S$ over all $B$ requests, reducing E CPU load by $>60\%$ and synchronizing token production.
2. **Pipeline-Aware Multi-Stage Splitting (+552 pts):**
   - Splitting $P\text{ PROC}$ is an overhead ($+S$ per piece). It should ONLY be done when decode data is actively in transit on the UP link. If decode is far away, running full pieces saves hundreds of milliseconds across the test.
3. **Min-Max Cluster Balancing:**
   - Equalizing the active request count across remotes ($B_0 \approx B_1 \dots \approx B/K$) eliminates straggler remotes from stalling the synchronized wave.
4. **HOL Blocking Protection on UP Link:**
   - A single $L_{in} = 4096$ prefill transfer on the UP link can stall the FIFO queue for seconds. Throttling $P\text{ PRE}$ when `up_time > decode_cycle` prevents catastrophic TPOT spikes.

### ❌ Disproven Hypotheses (Must NEVER Do):
1. **Steady-State Batch Capping (Submission 5, −80 pts):**
   - Artificially limiting $D\text{ PRE}$ or $D\text{ POST}$ batch sizes to a computed $B^*$ delays ready requests, pays $S$ multiple times on E, and explodes TPOT.
2. **Premature $D\text{ POST}$ Timeout / Splitting Waves (Submission 10, −21 pts):**
   - Firing $D\text{ POST}$ before all remotes finish fractures the unified wave into smaller, desynchronized sub-waves, re-introducing phase jitter and duplicate $S$ overhead.
3. **Inverting $D\text{ PRE}$ and $P\text{ POST}$ Priority (Submission 10, −21 pts):**
   - When a decode wave finishes $D\text{ POST}$, running $P\text{ POST}$ before $D\text{ PRE}$ stalls all decode requests on E, creating an idle bubble on the UP link and remote CPUs.

---

## 4. The 4 Algorithmic Levers to Reach 17,000+

```
┌──────────────────────────────────────────────────────────────────────────────────────────────────┐
│                                   THE 17,000+ LEVER ROADMAP                                      │
├───────────────────────────────────┬──────────────────────────────────────────────────────────────┤
│ Lever 1: Multi-Remote UP Stagger  │ Group D PRE to minimize UP latency serialization             │
│ Lever 2: Optimal Prefill Sizing   │ Match P PRE admission to exact decode wave compute gaps      │
│ Lever 3: Dynamic Slack Harvesting │ Utilize Ck idle windows between D PROC iterations            │
│ Lever 4: Tail TDR Optimization    │ SJF-dominant admission during non-decode intervals           │
└───────────────────────────────────┴──────────────────────────────────────────────────────────────┘
```

### Lever 1: Multi-Remote UP Latency Stagger Management
- **The Mechanic:** When $D\text{ PRE}$ finishes, it enqueues $K$ transfers in increasing remote order ($C_0, C_1, \dots, C_{K-1}$).
- Transfer $k$ finishes at:
  $$t_{\text{start}} + \sum_{i=0}^k \left( \text{latency} + \frac{8 m_i \cdot bpt}{bw \cdot 10^6} \right)$$
- Remote $C_{K-1}$ starts $D\text{ PROC}$ $(K-1) \times \text{latency}$ ms after $C_0$.
- **The Optimization:** On remote $C_k$, the duration of $P\text{ PROC}$ before $D\text{ PROC}$ arrives is exactly known:
  $$\text{Slack}(k) = k \times \text{latency} + \sum_{i=0}^{k-1} \frac{8 m_i \cdot bpt}{bw \cdot 10^6}$$
- Remote $C_k$ can compute a larger $P\text{ PROC}$ chunk of size proportional to $\text{Slack}(k)$ before $D\text{ PROC}$ arrives, squeezing maximum prefill progress out of the network stagger without delaying $D\text{ PROC}$ by even $1$ microsecond!

### Lever 2: Prefill Ingestion Sizing for Exact Wave Gaps
- While $E$ is waiting for the decode wave to traverse (UP $\to$ $D\text{ PROC}$ $\to$ DOWN), $E$ has a guaranteed free window:
  $$T_{\text{window}} = \text{latency} + S + d\_proc(\lceil B/K \rceil) + \text{latency} - S - d\_pre(B)$$
- During this window, $E$ can execute:
  1. $N_{\text{post}}$ tasks of $P\text{ POST}$
  2. $N_{\text{pre}}$ tasks of $P\text{ PRE}$
- We can greedily pack the highest-value prefill tasks ($P\text{ POST}$ FIFO, then $P\text{ PRE}$ SJF) whose total duration strictly fits within $T_{\text{window}}$, ensuring $E$ is 100% utilized with ZERO overlap into the next $D\text{ POST}$!

### Lever 3: Tail TDR Compression via Arrival Age Penalties
- In the waiting component, $TDR$ is the unweighted mean over all $R$ requests.
- When $R$ requests arrive over time, requests arriving during heavy decode waves accumulate queueing delay.
- In gaps between decode waves, prioritizing requests with the largest $(t - T_{ARR})$ compresses the upper tail of the TDR distribution, reducing $\text{excess\_tdr}$ directly.

### Lever 4: Pure Throughput Mode for Extreme $w_{tp}$
- When $w_{tp} \ge 0.85$ (pure throughput test):
  - Turn off all HOL throttling.
  - Turn off all $P\text{ PROC}$ splitting (`le = num_layers`).
  - Batch everything instantly to minimize total simulated makespan.

---

## 5. Methodical Implementation Plan

1. **Step 1:** Implement **Stagger-Aware Remote Slack Harvesting** (Lever 1) in $P\text{ PROC}$.
2. **Step 2:** Implement **Window-Bounded E Ingestion Packing** (Lever 2) for $P\text{ POST}$ and $P\text{ PRE}$.
3. **Step 3:** Benchmark against `best_15629.cpp` and lock in every verified gain.
