# What Worked & What Didn't Work (Root Cause Post-Mortem Log)

---

## 1. What Worked (Validated Invariants)

1. **Dual-Stage Wave Synchronization (+1,838 pts, Sub 8):**
   - Coalescing $D\text{ POST}$ on E when `pending_down_dec > 0` and $D\text{ PRE}$ when `decode_progressing > 0` eliminates decode wave phase jitter and aligns multi-node batches.
2. **Pipeline-Aware Even-Partition Slicing (+553 pts, Sub 9):**
   - Splitting $P\text{ PROC}$ into equal layer chunks rather than arbitrary 1-layer slices prevents starvation while minimizing fixed overhead $S$.
3. **Min-Max Uniform Remote Load Balancing (+553 pts, Sub 9):**
   - Equalizing active request count per remote $C_k$ ensures all remotes process decode requests in lockstep.
4. **Micro-Batch $P\text{ POST}$ Coalescing (+0.615 pts, Sub 13):**
   - Yielding E to $P\text{ POST}$ when `ready_d_pre.size() < K` allows prefill requests to join the upcoming decode wave with zero latency overhead.
5. **Continuous Weight-Aware HOL Throttle:**
   - Throttling $P\text{ PRE}$ when `up_time > decode_cycle * (1.0 + 3.0 * w_tp)` prevents huge prefill inputs from blocking critical decode tokens on the unidirectional network link.
6. **Strict Strategic Idling on E (`idle_for_down`):**
   - Idling E when decode DOWN transfers are in flight (`pending_down_dec > 0 && w_c >= 0.50`) guarantees E is ready to execute $D\text{ POST}$ immediately upon token arrival.
7. **QoS-Aware Round-Trip Decode Pipeline Pacing (+226.6 pts, Sub 20 Candidate):**
   - Matching remote $P\text{ PROC}$ chunk duration to the physical round-trip decode latency ($\Delta T = 2 \cdot \text{transfer} + 2 \cdot S + d\_post + d\_pre$) on $K=2$ and deep models on ultra-fast networks ($bw \ge 80, L \ge 32$) eliminates token queueing on remote workers, driving TPOT down to nominal hardware limits while achieving 100% Zero-Regression across all 50 stress tests.

---

## 2. What Didn't Work (Post-Mortem of Regressions)

1. **Submission 4 (-79.511 pts): Static $B_{opt}$ batch capping.**
   - Arbitrary batch-size caps throttled natural multi-remote saturation.
2. **Submission 7 (-0.844 pts): Hard pipeline throttle `prefill_in_pipeline >= 2`.**
   - Starved remotes during transient decode lulls.
3. **Submission 10 (-21.372 pts): Priority inversion on E.**
   - Delaying $D\text{ POST}$ to run $P\text{ PRE}$ caused severe tail-latency penalty ($Score_c \to 0$).
4. **Submission 11 (-2.067 pts): Static $k \times \text{latency}$ remote stagger.**
   - Artificially desynchronized remote CPU completions.
5. **Submission 14 (-63.568 pts): Pure Time-Based SRPT on Remotes.**
   - Preempted 90%-finished requests (1 layer left) for newly arrived short jobs, inflating TDR.
6. **Submission 15 (-0.334 pts): Strategic Idling blocking $P\text{ POST}$.**
   - Blocking $P\text{ POST}$ when DOWN was in flight increased input latency unnecessarily.
7. **Submission 17 (-258.748 pts): Gating $P\text{ POST}$ execution via decode window.**
   - Gating $P\text{ POST}$ directly inflated TDR on all requests.
8. **Submission 18 (-55.733 pts): Relaxing `idle_for_down` for $K < 8$.**
   - When E ran $P\text{ PRE}$ while decode DOWN was in flight, E was busy when tokens arrived, adding $+100\text{ ms}$ of phase jitter to TPOT.
9. **Submission 19 (-44.014 pts): Synthetic `decode_arrival_time` ignoring Network Link FIFO Queueing.**
   - Assuming idle UP links led to underestimated arrival times, triggering hyper-fragmentation into 1-layer chunks and burning $\approx 128\text{ ms}$ of remote CPU time in fixed schedule overhead $S$.
10. **Submission 20 (-0.322 pts): Narrow topology condition `(K == 2 || (bw >= 80.0 && num_layers >= 32))`.**
    - Slicing prefill based on static cluster thresholds created small phase mismatches when decode arrivals varied dynamically across bursts. Hardcoded topology guards fail to generalize to arbitrary contest test sets. Fundamental, queue-state-driven architecture is required.
11. **Submission 21 (-53.706 pts): Forward predictive MPC state tracking with residual tail losses.**
    - Relying on predictive link simulation (`up_link_free_at`, `down_link_free_at`) to relax E's `idle_for_down` and slice $P\text{ PROC}$ produced large wins on balanced tests (+881 pts locally) but suffered severe tail regressions on asymmetric/high-overhead tests (Test 26: -24.7 pts, Test 36: -9.1 pts, Test 19: -7.7 pts). On Codeforces, the aggregate score dropped from 15,630.115 to 15,576.409.
    - **Key Rule Enforcement:** Any solution that fails the Zero-Regression Invariant (`Max Loss > 0` or negative tail regressions) MUST NOT be submitted, regardless of total positive delta. Strict adherence to zero-regression is mandatory.
12. **Submission 22 (-17.135 pts): Hardcoded `is_fast_cluster (K >= 2 && bw >= 15.0 && S <= 2.0)` threshold guard.**
    - Slicing prefill with `target_dur = 2*S` on clusters with `bw >= 15.0` performed exceptionally well on deep models ($L \ge 32$) but hyper-fragmented short-layer models ($L \in \{4, 8\}$) into 1-layer micro-pieces, burning ~1.6 seconds of remote CPU time in fixed schedule overhead $S$. On Codeforces, the aggregate score dropped from 15,630.115 to 15,612.980.
    - **Key Architectural Rule:** NEVER use hardcoded topology thresholds (e.g. `bw >= 15.0`). All piece duration decisions must be governed by continuous physics-based invariants where piece compute time is bounded below by $\ge 3.0 \cdot (S + 2\cdot \text{latency})$.
