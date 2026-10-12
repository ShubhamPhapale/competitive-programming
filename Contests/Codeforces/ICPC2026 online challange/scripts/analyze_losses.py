import sys
import os
import math
from simulator import simulate_test

def analyze_all():
    tests = [f"local_tests/test_{i}.in" for i in range(1, 23)]
    
    print(f"{'Test':<5} | {'K':<2} | {'w_tp':<5} | {'w_c':<5} | {'Score':<8} | {'tp_comp':<8} | {'wait_comp':<9} | {'TDR / SLO1':<18} | {'TPOT / SLO2':<18} | {'Bottleneck'}")
    print("-" * 120)
    
    for idx, test_file in enumerate(tests, 1):
        with open(test_file, 'r') as f:
            data = f.read()
        lines = data.strip().split('\n')
        line1 = lines[0].split()
        K = int(line1[0])
        line2 = lines[1].split()
        SLO1 = float(line2[0])
        SLO2 = float(line2[1])
        w_tp = float(line2[5])
        w_c = float(line2[6])
        
        res = simulate_test(data, "./solution")
        
        score = res['score']
        tp_c = res['comp_tp'] * 1000
        wc_c = res['comp_c'] * 1000
        tdr_str = f"{res['mean_tdr']:.1f}/{SLO1:.1f}"
        tpot_str = f"{res['mean_tpot']:.1f}/{SLO2:.1f}"
        
        bottlenecks = []
        if res['comp_tp'] < 0.5:
            bottlenecks.append("LOW_TP")
        if res['mean_tdr'] > SLO1:
            bottlenecks.append(f"TDR_FAIL(+{(res['mean_tdr']-SLO1)/SLO1*100:.0f}%)")
        if res['mean_tpot'] > SLO2:
            bottlenecks.append(f"TPOT_FAIL(+{(res['mean_tpot']-SLO2)/SLO2*100:.0f}%)")
        b_str = ", ".join(bottlenecks) if bottlenecks else "CLEAN_OPTIMAL"
        
        print(f"#{idx:<4} | {K:<2} | {w_tp:<5.2f} | {w_c:<5.2f} | {score:<8.1f} | {tp_c:<8.1f} | {wc_c:<9.1f} | {tdr_str:<18} | {tpot_str:<18} | {b_str}")

if __name__ == "__main__":
    analyze_all()
