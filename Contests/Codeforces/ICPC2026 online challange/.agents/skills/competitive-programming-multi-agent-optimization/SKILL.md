---
name: competitive-programming-multi-agent-optimization
description: Specialized skill for competitive programming, heuristic contests, and distributed systems optimization using multi-agent parallel research, mathematical decomposition, systems pipeline auditing, and empirical zero-regression iteration.
---

# Multi-Agent Parallel Research & Optimization Skill for Competitive Programming

This skill defines the exact protocol and methodology for solving complex heuristic, interactive, or systems-level competitive programming challenges (such as ICPC Online Challenges, ICFPC, AtCoder Heuristic Contests, and Codeforces Marathon Contests) by spawning parallel specialized subagents, conducting deep multi-perspective mathematical and systems audits, and iteratively driving scores to top-of-the-leaderboard levels without regressions.

---

## 1. Core Architecture & Workflow

```
                        ┌────────────────────────────────────────┐
                        │      Lead Coordinator / Meta-Agent     │
                        │   (Maintains Invariants & Best Check)  │
                        └───────────────────┬────────────────────┘
                                            │
               ┌────────────────────────────┴────────────────────────────┐
               ▼                                                         ▼
┌───────────────────────────────┐                         ┌───────────────────────────────┐
│     Subagent 1 (Math/Algo)    │                         │    Subagent 2 (Systems/Perf)  │
│  - Objective Function Math    │                         │  - Pipeline State & Timing    │
│  - Theoretical Bounds & SLOs  │                         │  - Zero-Interference Windows  │
│  - Failure Mode Post-Mortems  │                         │  - Micro-Latency Elimination  │
└──────────────┬────────────────┘                         └───────────────┬───────────────┘
               │                                                         │
               └────────────────────────────┬────────────────────────────┘
                                            │
                                            ▼
                        ┌────────────────────────────────────────┐
                        │    Synthesized Solution Architecture   │
                        │   (Tested with -Wall -Wextra -Werror)  │
                        └───────────────────┬────────────────────┘
                                            │
                                            ▼
                        ┌────────────────────────────────────────┐
                        │         Contest Judge Submission       │
                        │   (Golden Rule: Rollback on Regress)   │
                        └────────────────────────────────────────┘
```

---

## 2. The 5-Phase Multi-Agent Optimization Protocol

### Phase 1: Exhaustive Problem & Benchmark Anatomy
1. **Objective Function Dissection:**
   - Decompose total score into its constituent components (e.g., $w_{\text{tp}}$ Throughput vs. $w_c$ Waiting Time / SLO compliance).
   - Identify nonlinearities, Euclidean distance penalties ($L_2$ norms), threshold discontinuities (e.g., $dist_{\text{base}} = 0$), and clamping functions.
2. **Deterministic Hardware & Network Cost Model:**
   - Map all shared serial resources (CPU, GPU, network links, FIFO queues).
   - Write closed-form transfer and execution duration formulas:
     $$T_{\text{xfer}}(\text{len}) = \text{latency} + \frac{8 \cdot \text{len} \cdot \text{bpt}}{\text{bw} \times 10^6}$$

---

### Phase 2: Parallel Multi-Perspective Subagent Dispatch
When a score plateaus or a major leap is needed, spawn at least two specialized research subagents using `invoke_subagent`:

1. **Subagent 1: Mathematical & Algorithmic Strategist (`Role: Mathematical Algorithm Specialist`):**
   - **Mandate:** Analyze the mathematical bounds, queuing theory behavior, priority inversion risks, and theoretical optimality of scheduling heuristics (e.g., SRPT, SJF, FIFO).
   - **Post-Mortem Analysis:** For any previous regressions, mathematically identify the root cause (e.g., starvation of nearly-finished requests under time-based SRPT vs. layer-count SRPT).

2. **Subagent 2: Systems Pipeline Architect (`Role: Systems Pipeline Architect`):**
   - **Mandate:** Audit every line of the event ingestion loop, finite state machine (FSM), memory allocator overhead, and hardware pipeline timelines.
   - **Timeline Modeling:** Identify natural idle windows (e.g., the $15\text{–}35\text{ ms}$ interval where the primary computer is idle while a wave traverses the cloud network/remotes) and formulate exact zero-interference conditions for background task packing.

---

### Phase 3: Synthesis & Zero-Interference Optimization
Combine the subagents' mathematical and systems insights into concrete, actionable upgrades:

1. **Dual-Stage Wave Synchronization:**
   - Synchronize distributed workers at both ingress ($D\text{ PRE}$) and egress ($D\text{ POST}$) to eliminate phase jitter and amortize fixed schedule costs $S$.
2. **Predictive Event-Horizon Simulation:**
   - Track forward timelines (`up_busy_until`, `down_busy_until`, `est_decode_return_time`) to know the exact arrival time of future events.
3. **Strategic Idling:**
   - Ingest background tasks only when $t_{\text{now}} + T_{\text{task}} \le t_{\text{event\_return}} - \epsilon$. Otherwise, deliberately idle to allow high-priority tasks to fire with zero queueing jitter.
4. **Feasible-Set SJF Admission:**
   - Search the feasible subset of tasks that satisfy HOL throttling to keep resources busy without blocking shared queues.

---

### Phase 4: Verification & Golden Rule Rollback Protocol
1. **Compilation Invariant:**
   - Always compile locally with `g++ -std=c++17 -O2 -Wall -Wextra -Werror` to guarantee zero warnings, zero undefined behavior, and optimal performance.
2. **The Golden Rule:**
   - **Frozen Checkpoint:** Before testing a new candidate, ensure the current highest score is saved as `best_<score>.cpp`.
   - **Immediate Rollback:** If a submission scores lower than the benchmark, *immediately* revert `solution.cpp` from `best_<score>.cpp`.
   - **Post-Mortem Recording:** Document the exact difference between expected hypothesis and empirical reality in `workednotworkedtillnow.md`.
   - **Advancement:** Only update the best checkpoint when a submission strictly exceeds the previous peak.

---

### Phase 5: Documentation Integrity
Always maintain complete, synchronized documentation across:
- `progress.md`: Chronological table of all submissions with exact scores, runtimes, memory, and deltas.
- `workednotworkedtillnow.md`: Comprehensive log of verified breakthroughs vs. disproven hypotheses.
- `explanation.md`: Full architectural and mathematical breakdown of the current active solution.
- `deep_analysis_v2.md`: Deep technical roadmap and mathematical bounds analysis.

---

## 3. Example Prompt Template for Subagent Spawning

```json
{
  "Subagents": [
    {
      "TypeName": "research",
      "Role": "Mathematical Algorithm Specialist",
      "Model": "inherit",
      "Prompt": "Conduct an exhaustive mathematical and algorithmic analysis of the problem. Review problem.txt (scoring math, SLOs, clamp functions), workednotworkedtillnow.md (full history of successes and regressions), and current best solution.cpp. Report: (1) Exact mathematical breakdown of current score deficit, (2) Root cause of recent regressions, (3) High-confidence mathematical improvements to reach top tier."
    },
    {
      "TypeName": "research",
      "Role": "Systems Pipeline Architect",
      "Model": "inherit",
      "Prompt": "Perform an exhaustive systems and pipeline audit of solution.cpp. Review problem.txt and deep_analysis_v2.md. Report: (1) Detailed line-by-line audit of FSM, queues, and task dispatch, (2) Identification of hidden micro-latencies or idle bubbles, (3) Exact pipeline timeline during cloud roundtrips and zero-interference task packing rules, (4) Concrete code implementation plan."
    }
  ]
}
```
