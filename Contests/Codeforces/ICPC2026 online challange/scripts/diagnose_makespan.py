import sys
from simulator import simulate_test

with open("local_tests/test_2.in", "r") as f:
    data = f.read()

# Let's inspect when tasks are executed on E and C0..C7
import subprocess
p = subprocess.Popen(["./solution"], stdin=subprocess.PIPE, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True)

# We can run simulator with logging
# Let's see what events happen in the first 2000 ms of test 2
from simulator import Interactor

with open("local_tests/test_2.in", "r") as f:
    lines = [line.strip() for line in f if line.strip()]

interactor = Interactor(lines, "./solution")
events_log = []

while not interactor.done():
    # step interactor
    curr_t = interactor.current_time
    # count active states
    states = {}
    for r in interactor.requests:
        s = r.state
        states[s] = states.get(s, 0) + 1
    if len(events_log) < 50 or curr_t > 12000:
        events_log.append((curr_t, interactor.E_busy_until, states))
    
    # run step
    if not interactor.step():
        break

print(f"Total steps: {len(events_log)}, Finished at t = {interactor.current_time:.1f}")
for t, e_busy, st in events_log[:20]:
    print(f"t={t:7.1f} | E_busy={e_busy:7.1f} | States: {st}")
print("...")
for t, e_busy, st in events_log[-10:]:
    print(f"t={t:7.1f} | E_busy={e_busy:7.1f} | States: {st}")
