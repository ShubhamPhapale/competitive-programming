import os
import subprocess
import re

def run_benchmark(name):
    print(f"[{name}]")
    res = subprocess.run(f"python3 benchmark.py best_15630 {name}", shell=True, capture_output=True, text=True)
    
    delta = None
    regression = None
    
    for line in res.stdout.split('\n'):
        if 'Total Sum of Scores' in line:
            m = re.search(r'\(\+?\s*([-0-9.]+)\s*pts total delta\)', line)
            if m:
                delta = m.group(1)
            else:
                m = re.search(r'\(-\s*([0-9.]+)\s*pts total delta\)', line)
                if m:
                    delta = "-" + m.group(1)
        if 'Total Regression Sum' in line:
            m = re.search(r'([-0-9.]+)\s*pts', line)
            if m:
                regression = m.group(1)
                
    if regression is None and delta is not None:
        regression = "0.000"
                
    print(f"Delta: {delta}")
    print(f"Regression: {regression}\n")

for th in ["040", "030", "020", "010", "00"]:
    run_benchmark(f"varA_{th}")

for cond in ["1_5S", "S_plus_1", "0"]:
    run_benchmark(f"varB_{cond}")
