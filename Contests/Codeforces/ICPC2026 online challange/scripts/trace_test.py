import sys
from simulator import simulate_test

with open("local_tests/test_2.in", "r") as f:
    data = f.read()

# Let's inspect the requests and parameters in test 2
lines = data.strip().split('\n')
print("Config:", lines[0])
print("Scoring:", lines[1])
print("N:", lines[2])
R_idx = 3 + int(lines[2])
R = int(lines[R_idx])
print("R (requests):", R)
for i in range(R_idx + 1, min(R_idx + 11, len(lines))):
    print(" Req:", lines[i])
