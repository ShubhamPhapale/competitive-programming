import os
import subprocess

def run_variant(name, code, benchmark_cmd):
    print(f"--- Compiling {name} ---")
    with open(f"{name}.cpp", "w") as f:
        f.write(code)
    
    subprocess.run(["g++", "-std=c++17", "-O2", f"{name}.cpp", "-o", name], check=True)
    print(f"--- Running benchmark for {name} ---")
    res = subprocess.run(benchmark_cmd, shell=True, capture_output=True, text=True)
    for line in res.stdout.split('\n'):
        if 'Delta:' in line or 'Total Regression:' in line or 'Total Improvement:' in line or 'Score:' in line:
            print(line)

with open("best_15630.cpp", "r") as f:
    base_code = f.read()

print("==== VARIANT A ====")
for th in ["0.40", "0.30", "0.20", "0.10", "0.0"]:
    name = f"varA_{th.replace('.', '')}"
    code = base_code.replace("w_c >= 0.50", f"w_c >= {th}")
    run_variant(name, code, f"python3 benchmark.py best_15630 {name}")

print("\n==== VARIANT B ====")
base_b = base_code.replace("int pending_down_dec = 0;", "int pending_down_dec = 0;\n    double down_free_time = 0.0;")
base_b = base_b.replace("pending_down_dec++;", "pending_down_dec++;\n                        down_free_time = max(current_time, down_free_time) + transfer_time(m);")

idle_logic_template = """bool idle_for_down = false;
                if (pending_down_dec > 0) {
                    if (w_c >= 0.50) idle_for_down = true;
                    else {
                        double time_until_dpost = down_free_time - current_time;
                        if (time_until_dpost < COND) idle_for_down = true;
                    }
                }"""

for cond_name, cond in [("1_5S", "S * 1.5"), ("S_plus_1", "S + 1.0"), ("0", "0")]:
    name = f"varB_{cond_name}"
    code = base_b.replace("bool idle_for_down = (pending_down_dec > 0 && w_c >= 0.50);", idle_logic_template.replace("COND", cond))
    run_variant(name, code, f"python3 benchmark.py best_15630 {name}")
