import sys
import subprocess

# Let's inspect test 2 execution events
with open("local_tests/test_2.in", "r") as f:
    data = f.read()

lines = data.strip().split('\n')
print("Config:", lines[0])
print("Scoring:", lines[1])
R_idx = 3 + int(lines[2])
R = int(lines[R_idx])
print("R requests:", R)
tot_tokens = sum(int(lines[i].split()[2]) for i in range(R_idx+1, R_idx+1+R))
print("Total output tokens:", tot_tokens)
