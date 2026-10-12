import os
import subprocess
import shutil

baseline = "best_15630.cpp"
orig_line = "            bool hold_d_post = (pending_down_dec > 0);"

variants = {
    "A_no_hold": "            bool hold_d_post = false;",
    "B_hold_many": "            bool hold_d_post = (pending_down_dec > 1);",
    "C_weight_aware": "            bool hold_d_post = (pending_down_dec > 0 && w_c < 0.50);",
    "D_inv_weight_aware": "            bool hold_d_post = (pending_down_dec > 0 && w_tp < 0.50);",
    "E_always": "            bool hold_d_post = false; // E is effectively A since it's already PRIORITY 1"
}

with open(baseline, "r") as f:
    code = f.read()

results = {}

for name, new_line in variants.items():
    cand_file = f"cand_{name}.cpp"
    new_code = code.replace(orig_line, new_line)
    
    with open(cand_file, "w") as f:
        f.write(new_code)
        
    print(f"Compiling {cand_file}...")
    subprocess.run(["g++", "-std=c++17", "-O2", cand_file, "-o", cand_file.replace('.cpp', '')])
    
    print(f"Benchmarking {cand_file}...")
    res = subprocess.run(
        ["python3", "benchmark.py", "best_15630", cand_file.replace('.cpp', '')], 
        capture_output=True, text=True
    )
    
    # parse the output
    out = res.stdout
    lines = out.strip().split('\n')
    
    # We want to extract Exact total delta and regression sum
    # We will just print the tail of the benchmark output
    summary = "\n".join(lines[-15:])
    print(f"--- {name} ---")
    print(summary)
    print("--------------------------------------------------\n")
