# Multi-Agent Optimization Skill & Methodology Guide
## Competitive Programming, Heuristic Contests & Distributed Scheduling

This document serves as the complete operational specification and general skill for driving competitive programming and distributed systems schedulers to the top tier of competitive leaderboards using **Parallel Multi-Agent Research & Synthesis**.

---

## 1. The Multi-Agent Specialization Model

When tackling complex multi-objective heuristic problems (such as the Edge-Cloud Collaborative Scheduling Challenge), monolithic single-threaded analysis often misses critical trade-offs between queuing delays, CPU scheduling overhead, and network FIFO serialization. 

To overcome this, we employ a **two-pronged multi-agent parallel investigation model**:

```
                               ┌────────────────────────────────────────┐
                               │       Lead Meta-Agent Controller       │
                               │   (Synthesizer & Invariant Enforcer)   │
                               └───────────────────┬────────────────────┘
                                                   │
                      ┌────────────────────────────┴────────────────────────────┐
                      ▼                                                         ▼
       ┌───────────────────────────────┐                         ┌───────────────────────────────┐
       │   Subagent 1: Math Specialist │                         │ Subagent 2: Systems Architect │
       ├───────────────────────────────┤                         ├───────────────────────────────┤
       │ • Objective Function & Clamps │                         │ • Hardware Pipeline Timeline  │
       │ • L2 Metric & SLO Knife-Edges │                         │ • Queue Serializations (FIFO) │
       │ • Theoretical Bounds (tp_UB)  │                         │ • Idle Window Identification  │
       │ • Regression Post-Mortems     │                         │ • Zero-Interference Packing   │
       └──────────────┬────────────────┘                         └───────────────┬───────────────┘
                      │                                                         │
                      └────────────────────────────┬────────────────────────────┘
                                                   │
                                                   ▼
                               ┌────────────────────────────────────────┐
                               │    Synthesized Solution Architecture   │
                               │   (Tested with -Wall -Wextra -Werror)  │
                               └────────────────────────────────────────┘
```

---

## 2. Core Methodologies Discovered & Formalized

### A. Dual-Stage Wave Synchronization (+1,837.756 pts)
- **The Core Issue:** In multi-node systems with serial network links, asynchronous task completions fragment execution into $K$ desynchronized micro-tasks, multiplying fixed scheduling penalties ($S$) by $K \times$.
- **The Solution:** Two-stage barrier coalescing:
  1. `hold_d_post = (pending_down_dec > 0)`: Waits for all worker transfers to arrive before firing a unified output task.
  2. `hold_d_pre = (decode_progressing > 0)`: Merges newly admitted inputs into the unified wave.

### B. Predictive Event-Horizon Wave Tracking & Zero-Interference Idling
- **The Core Issue:** Starting low-priority background work (e.g. prefill admission) when a high-priority wave (e.g. decode output) is about to land locks shared resources, introducing phase jitter and inflating tail latency.
- **The Solution:** Exact forward deterministic modeling:
  $$t_{\text{wave\_return}} = \max_k (t_{\text{UP}}[k] + S + \text{duration}_{\text{proc}}[k]) + \sum T_{\text{DOWN}}[k]$$
  - Schedule background work **if and only if** $t_{\text{now}} + T_{\text{task}} \le t_{\text{wave\_return}} - \epsilon$.
  - Otherwise, **strategically idle** so the high-priority wave executes with zero latency.

### C. Starvation-Free Layer-Count SRPT
- **The Core Issue:** True time-based remaining time comparisons preempt partially-completed large tasks in favor of newly arrived small tasks, creating severe convoy starvation in multi-stage pipelines.
- **The Solution:** Layer-count SRPT ($\min(\text{num\_layers} - \text{current\_layer})$) ensures that requests that are $90\%$ complete are immediately flushed out of the pipeline, preventing tail latency explosion.

### D. Feasible-Set SJF Admission
- **The Core Issue:** Evaluating only the globally smallest task can cause total starvation if that single task is throttled to prevent head-of-line blocking.
- **The Solution:** Filter the queue for the **feasible subset** of unthrottled tasks, picking the shortest job among safe candidates.

---

## 3. The 5 Invariant Rules of Optimization (Golden Rule Protocol)

1. **Never Regress:** Always preserve the latest verified high-scoring binary as `best_<score>.cpp`.
2. **Immediate Rollback:** If a submission scores lower, immediately revert `solution.cpp` from `best_<score>.cpp`.
3. **Exhaustive Post-Mortem:** Record every failed experiment in `workednotworkedtillnow.md` explaining *Expected vs. Reality*.
4. **Zero-Warning Strict Builds:** Always compile with `g++ -std=c++17 -O2 -Wall -Wextra -Werror`.
5. **Continuous Documentation Sync:** Keep `progress.md`, `workednotworkedtillnow.md`, and `explanation.md` 100% updated after every iteration.
