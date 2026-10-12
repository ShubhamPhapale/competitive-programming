import glob, os, re, subprocess

test_files = sorted(glob.glob("local_tests/test_*.in"), key=lambda x: int(re.search(r"test_(\d+)", x).group(1)))

rows = []
for tf in test_files:
    tid = int(re.search(r"test_(\d+)", tf).group(1))
    with open(tf) as f:
        lines = [l.strip() for l in f if l.strip()]
    k_line = lines[0].split()
    K = int(k_line[0])
    S = float(k_line[1])
    lat = float(k_line[2])
    bw = float(k_line[3])
    bpt = int(k_line[4])
    layers = int(k_line[5])
    
    slo_line = lines[1].split()
    slo1 = float(slo_line[0])
    slo2 = float(slo_line[1])
    dist_base = float(slo_line[4])
    wtp = float(slo_line[5])
    wc = float(slo_line[6])
    
    N = int(lines[2])
    
    cmd = f"python3 simulator.py \"{tf}\" \"./best_15630\""
    res = subprocess.run(cmd, shell=True, stdout=subprocess.PIPE, text=True)
    out = res.stdout.strip().splitlines()[-1]
    
    m = re.search(r"Score:\s+([\d\.]+)\s+\(tp_comp:\s+([\d\.]+),\s+wait_comp:\s+([\d\.]+),\s+TDR:\s+([\d\.]+)ms,\s+TPOT:\s+([\d\.]+)ms,\s+makespan:\s+([\d\.]+)ms\)", out)
    if m:
        score, tp_comp, wait_comp, tdr, tpot, ms = map(float, m.groups())
        rows.append({
            "tid": tid, "K": K, "S": S, "lat": lat, "bw": bw, "layers": layers,
            "slo1": slo1, "slo2": slo2, "dist_base": dist_base, "wtp": wtp, "wc": wc,
            "N": N, "score": score, "tp_comp": tp_comp, "wait_comp": wait_comp,
            "tdr": tdr, "tpot": tpot, "ms": ms
        })

zero_wc = [r for r in rows if r["wait_comp"] == 0.0]
low_wc = [r for r in rows if 0.0 < r["wait_comp"] < 0.5]
high_wc = [r for r in rows if r["wait_comp"] >= 0.5]

print("--- Distribution of wait_comp ---")
avg_zero = sum(r["score"] for r in zero_wc)/len(zero_wc) if zero_wc else 0
avg_low = sum(r["score"] for r in low_wc)/len(low_wc) if low_wc else 0
avg_high = sum(r["score"] for r in high_wc)/len(high_wc) if high_wc else 0
print(f"wait_comp == 0.0 (Score_c = 0): {len(zero_wc)} tests (Average Score: {avg_zero:.1f})")
print(f"0.0 < wait_comp < 0.5: {len(low_wc)} tests (Average Score: {avg_low:.1f})")
print(f"wait_comp >= 0.5: {len(high_wc)} tests (Average Score: {avg_high:.1f})")

print("\n--- Tests with wait_comp == 0.0 (Score_c = 0) ---")
for r in zero_wc:
    tid = r['tid']
    K = r['K']
    S = r['S']
    bw = r['bw']
    lat = r['lat']
    layers = r['layers']
    wtp = r['wtp']
    wc = r['wc']
    slo1 = r['slo1']
    tdr = r['tdr']
    slo2 = r['slo2']
    tpot = r['tpot']
    db = r['dist_base']
    sc = r['score']
    print(f"Test #{tid:2d}: K={K}, S={S:.1f}, bw={bw:5.1f}, lat={lat:4.1f}, L={layers:2d}, wtp={wtp:.2f}, wc={wc:.2f}, SLO1={slo1:6.1f}, TDR={tdr:6.1f}, SLO2={slo2:5.2f}, TPOT={tpot:5.2f}, db={db:5.2f}, score={sc:5.1f}")
