import subprocess
import re

code_template = open("test_tracker2.cpp").read()

def test_threshold(thresh_str):
    print(f"Testing condition: {thresh_str}")
    
    old_idle = """            if (pending_down_dec > 0) {
                if (w_c >= 0.50) idle_for_down = true; 
                else {
                    double time_until_dpost = down_free_time - current_time;
                    if (time_until_dpost < S + 5.0) {
                        idle_for_down = true;
                    }
                }
            }"""
            
    new_idle = f"""            if (pending_down_dec > 0) {{
                if (w_c >= 0.50) idle_for_down = true; 
                else {{
                    double time_until_dpost = down_free_time - current_time;
                    if ({thresh_str}) {{
                        idle_for_down = true;
                    }}
                }}
            }}"""
            
    code = code_template.replace(old_idle, new_idle)
    with open("temp_test.cpp", "w") as f:
        f.write(code)
        
    subprocess.run("g++ -std=c++17 -O2 -o temp_test temp_test.cpp", shell=True)
    res = subprocess.run("python3 benchmark.py best_15630 temp_test", shell=True, stdout=subprocess.PIPE, text=True)
    
    # Extract total delta and regression
    total_delta = 0.0
    reg_sum = 0.0
    for line in res.stdout.split('\n'):
        if "Total Sum of Scores" in line:
            m = re.search(r'\(\+?\s*([-0-9.]+)\s*pts total delta\)', line)
            if m: total_delta = float(m.group(1))
        if "Total Regression Sum" in line:
            m = re.search(r':\s*([-0-9.]+)\s*pts', line)
            if m: reg_sum = float(m.group(1))
            
    print(f"  Delta: {total_delta:+.3f}  |  Regression: {reg_sum:+.3f}")
    return total_delta, reg_sum

conditions = [
    "time_until_dpost < S + 1.0",
    "time_until_dpost < S + 2.0",
    "time_until_dpost < S",
    "time_until_dpost < S * 1.5",
    "time_until_dpost < 0.0",  # Meaning it already arrived
    "false", # No idling for w_c < 0.50
    "time_until_dpost < S + 5.0 && w_c >= 0.25",
    "time_until_dpost < S && w_c >= 0.25"
]

for cond in conditions:
    test_threshold(cond)

