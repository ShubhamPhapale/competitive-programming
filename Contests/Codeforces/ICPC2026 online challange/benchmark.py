import subprocess
import os
import sys
import glob
import re
import math
from concurrent.futures import ProcessPoolExecutor

def run_single_test(args):
    test_file, binary_a, binary_b = args
    bin_a = binary_a if ("/" in binary_a or "\\" in binary_a) else f"./{binary_a}"
    bin_b = binary_b if ("/" in binary_b or "\\" in binary_b) else f"./{binary_b}"
    cmd_a = f"python3 simulator.py '{test_file}' '{bin_a}'"
    cmd_b = f"python3 simulator.py '{test_file}' '{bin_b}'"
    
    res_a = subprocess.run(cmd_a, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    res_b = subprocess.run(cmd_b, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)
    
    score_a = 0.0
    score_b = 0.0
    for line in res_a.stdout.splitlines():
        if "Score:" in line:
            try:
                score_a = float(line.split("Score:")[1].split()[0])
            except Exception:
                pass
    for line in res_b.stdout.splitlines():
        if "Score:" in line:
            try:
                score_b = float(line.split("Score:")[1].split()[0])
            except Exception:
                pass
                
    m = re.search(r"test_(\d+)\.in", test_file)
    t_id = int(m.group(1)) if m else 0
    return (t_id, score_a, score_b)

def main():
    if len(sys.argv) < 3:
        print("Usage: python3 benchmark.py <binary_a> <binary_b>")
        sys.exit(1)
        
    binary_a = sys.argv[1]
    binary_b = sys.argv[2]
    
    test_files = sorted(glob.glob("local_tests/test_*.in"), key=lambda x: int(re.search(r"test_(\d+)\.in", x).group(1)))
    if not test_files:
        print("No test files found in local_tests/. Run python3 generate_tests.py first.")
        sys.exit(1)
        
    tasks = [(tf, binary_a, binary_b) for tf in test_files]
    
    print(f"=========================================================================================")
    print(f"   COMPREHENSIVE STATISTICAL GENERALIZATION BENCHMARK (N={len(test_files)} Diverse Topologies)   ")
    print(f"   Binary A (Baseline) : {binary_a}")
    print(f"   Binary B (Candidate): {binary_b}")
    print(f"=========================================================================================\n")
    
    with ProcessPoolExecutor(max_workers=min(16, os.cpu_count() or 4)) as executor:
        results = list(executor.map(run_single_test, tasks))
        
    results.sort(key=lambda x: x[0])
    
    tot_a = 0.0
    tot_b = 0.0
    wins = 0
    losses = 0
    ties = 0
    regressions = []
    improvements = []
    log_ratio_sum = 0.0
    valid_ratios = 0
    deltas = []
    
    TOLERANCE = 0.05  # ±0.05 pts is treated as tie due to floating point timing
    
    print(f"{'Test':<6} | {'Binary A (' + binary_a + ')':<21} | {'Binary B (' + binary_b + ')':<21} | {'Delta (B - A)':<16} | {'Status'}")
    print("-" * 85)
    
    for t_id, s_a, s_b in results:
        tot_a += s_a
        tot_b += s_b
        delta = s_b - s_a
        deltas.append(delta)
        sign = "+" if delta >= 0 else ""
        
        status = "TIE"
        if delta > TOLERANCE:
            status = "WIN  (+" + f"{delta:.2f}" + ")"
            wins += 1
            improvements.append((t_id, delta))
        elif delta < -TOLERANCE:
            status = "LOSS (" + f"{delta:.2f}" + ")"
            losses += 1
            regressions.append((t_id, delta))
        else:
            ties += 1
            
        if s_a > 1e-6 and s_b > 1e-6:
            log_ratio_sum += math.log(s_b / s_a)
            valid_ratios += 1
            
        print(f"#{t_id:<5} | {s_a:10.3f} pts         | {s_b:10.3f} pts         | {sign}{delta:10.3f} pts | {status}")
        
    print("=" * 85)
    
    # ── Statistical Metrics Calculation ──────────────────────────────
    delta_tot = tot_b - tot_a
    sign_tot = "+" if delta_tot >= 0 else ""
    
    # 1. Geometric Mean Score Ratio (Scale-Invariant Generalization Metric)
    geo_mean_ratio = math.exp(log_ratio_sum / valid_ratios) if valid_ratios > 0 else 1.0
    geo_pct = (geo_mean_ratio - 1.0) * 100.0
    
    # 2. Trimmed Mean Delta (Robust to single-test outlier blowups)
    sorted_deltas = sorted(deltas)
    trim_k = max(1, int(len(deltas) * 0.05)) # 5% trim each side
    trimmed_deltas = sorted_deltas[trim_k : len(deltas) - trim_k]
    trimmed_mean = sum(trimmed_deltas) / len(trimmed_deltas) if trimmed_deltas else 0.0
    
    # 3. Regression Audit
    max_loss = min([d for _, d in regressions]) if regressions else 0.0
    tot_loss = sum([d for _, d in regressions]) if regressions else 0.0
    
    print("\n" + "─" * 40 + " BENCHMARK SUMMARY & GENERALIZATION AUDIT " + "─" * 40)
    print(f"  • Total Sum of Scores       : {tot_a:10.3f} pts  -->  {tot_b:10.3f} pts  ({sign_tot}{delta_tot:10.3f} pts total delta)")
    print(f"  • Average Score Per Test    : {tot_a/len(results):10.3f} pts  -->  {tot_b/len(results):10.3f} pts  ({sign_tot}{delta_tot/len(results):10.3f} pts/test)")
    print(f"  • Geometric Mean Ratio (GMR): {geo_mean_ratio:.6f}x  ({geo_pct:+.4f}% normalized scale-invariant gain)")
    print(f"  • 5% Trimmed Mean Delta     : {trimmed_mean:+.3f} pts/test (outlier-filtered robust delta)")
    print(f"  • Head-to-Head Record       : {wins} Wins | {ties} Ties | {losses} Losses  (Dominance: {wins/(losses+1e-5):.1f}:1)")
    print(f"  • Max Individual Loss       : {max_loss:10.3f} pts")
    print(f"  • Total Regression Sum      : {tot_loss:10.3f} pts")
    
    print("─" * 105)
    
    # ── Zero-Regression Gatekeeper Evaluation ─────────────────────────
    # Criteria for a valid submission:
    # 1. Wins >= 3 * Losses
    # 2. Max individual loss <= 1.0 pt (no major single-test collapse)
    # 3. Total regression sum >= -5.0 pts
    # 4. GMR > 1.0005 (at least +0.05% geometric gain across all tests)
    
    is_safe = (losses == 0 or (wins >= 3 * losses and abs(max_loss) <= 1.5 and abs(tot_loss) <= 5.0))
    has_meaningful_gain = (delta_tot >= 50.0 and geo_mean_ratio >= 1.001)
    
    print("   VERDICT: ", end="")
    if is_safe and has_meaningful_gain:
        print("✅ PASSED ROBUSTNESS & GENERALIZATION CRITERIA — SAFE FOR LIVE SUBMISSION!")
    elif not is_safe:
        print("❌ FAILED ZERO-REGRESSION INVARIANT — Overfitting or severe tail regression detected!")
        if regressions:
            print("      Regressed Test IDs:", [f"#{tid} ({loss:.2f} pts)" for tid, loss in regressions])
    else:
        print("⚠️ NEUTRAL — Delta too small or insufficient evidence of generalizable gain.")
    print("─" * 105 + "\n")

if __name__ == "__main__":
    main()
