import random
import os
import math

def generate_generalized_suite():
    os.makedirs("local_tests", exist_ok=True)
    
    # ── Test 1: Exact Public Example 1 ──────────────────────────────
    t1 = """1 1.000000000 2.000000000 1.000000000 125000 4
30.000000000 15.000000000 0.062500000 0.022222222 0.000000000 0.500000000 0.500000000
2
1 3.000000000 10.000000000 2.000000000 1.000000000 4.000000000 1.000000000
4 3.000000000 10.000000000 2.000000000 1.000000000 4.000000000 1.000000000
1
0.000000000 4 1"""
    with open("local_tests/test_1.in", "w") as f:
        f.write(t1)
        
    def build_test(test_id, K, S, latency, bw, bpt, num_layers, w_tp, w_c, requests, dist_base_override=None):
        N_sizes = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048, 4096]
        N = len(N_sizes)
        table_rows = []
        for bs in N_sizes:
            ppre = 0.5 + 0.05 * math.sqrt(bs)
            pproc = 2.0 + 0.1 * bs
            ppost = 0.5 + 0.05 * math.sqrt(bs)
            dpre = 0.5 + 0.02 * bs
            dproc = 1.0 + 0.08 * bs
            dpost = 0.5 + 0.02 * bs
            table_rows.append((bs, ppre, pproc, ppost, dpre, dproc, dpost))
            
        def xfer(length):
            return latency + 8.0 * float(length) * float(bpt) / (bw * 1e6)
        
        # Calculate baseline metrics via serial 1-at-a-time schedule
        t_ser = 0.0
        tdr_list_base = []
        tpot_gaps_base = []
        tot_tokens = sum(r[2] for r in requests)
        
        for arr, Lin, Lout in requests:
            t_start = max(arr, t_ser)
            t_ser = t_start + S + (0.5 + 0.05 * math.sqrt(Lin))
            t_ser += xfer(Lin)
            t_ser += S + (2.0 + 0.1 * Lin)
            t_ser += xfer(Lin)
            t_ser += S + (0.5 + 0.05 * math.sqrt(Lin))
            tdr_list_base.append(t_ser - arr)
            
            last_tok = t_ser
            for _ in range(Lout):
                t_ser += S + (0.5 + 0.02 * 1)
                t_ser += xfer(1)
                t_ser += S + (1.0 + 0.08 * 1)
                t_ser += xfer(1)
                t_ser += S + (0.5 + 0.02 * 1)
                if Lout > 1:
                    tpot_gaps_base.append(t_ser - last_tok)
                last_tok = t_ser
                
        earliest_arr = min(r[0] for r in requests)
        T_base = max(0.001, t_ser - earliest_arr)
        tp_base = float(tot_tokens) / T_base
        tp_UB = tp_base * (2.2 + 0.15 * K)
        
        mean_tdr_base = sum(tdr_list_base) / len(tdr_list_base)
        mean_tpot_base = sum(tpot_gaps_base) / len(tpot_gaps_base) if tpot_gaps_base else 20.0
        
        # Realistic parallel baseline targets matching Codeforces
        SLO1 = mean_tdr_base / (2.5 * float(K))
        SLO2 = mean_tpot_base * 0.6
        
        ex_tdr = max(0.0, (mean_tdr_base - SLO1) / SLO1)
        ex_tpot = max(0.0, (mean_tpot_base - SLO2) / SLO2)
        dist_base = (math.sqrt(ex_tdr**2 + ex_tpot**2) * 1.5) if dist_base_override is None else dist_base_override
        
        lines = []
        lines.append(f"{K} {S:.9f} {latency:.9f} {bw:.9f} {bpt} {num_layers}")
        lines.append(f"{SLO1:.9f} {SLO2:.9f} {tp_UB:.9f} {tp_base:.9f} {dist_base:.9f} {w_tp:.9f} {w_c:.9f}")
        lines.append(f"{N}")
        for row in table_rows:
            lines.append(f"{row[0]} " + " ".join(f"{v:.9f}" for v in row[1:]))
        lines.append(f"{len(requests)}")
        for req in requests:
            lines.append(f"{req[0]:.9f} {req[1]} {req[2]}")
            
        with open(f"local_tests/test_{test_id}.in", "w") as f:
            f.write("\n".join(lines))

    # Helper to generate requests
    def make_reqs(R, mode='streaming', Lin_range=(20, 1000), Lout_range=(1, 30), seed=42):
        random.seed(seed)
        reqs = []
        t = 0.0
        for i in range(R):
            if mode == 'burst':
                t = 0.0
            elif mode == 'streaming':
                t += random.uniform(5.0, 30.0)
            elif mode == 'periodic_pulse':
                if i % 20 == 0: t += 200.0
                else: t += random.uniform(0.0, 5.0)
            elif mode == 'pareto_arrival':
                t += random.paretovariate(1.5) * 5.0
                
            if mode == 'pareto_len':
                Lin = min(4096, max(20, int(random.paretovariate(1.2) * 100)))
            else:
                Lin = random.randint(Lin_range[0], Lin_range[1])
                
            Lout = random.randint(Lout_range[0], Lout_range[1])
            reqs.append((t, Lin, Lout))
        return reqs

    # ═════════════════════════════════════════════════════════════════
    #  SUITE 1: Core Contest Spectrum (Tests 2 to 22)
    # ═════════════════════════════════════════════════════════════════
    build_test(2, K=8, S=2.0, latency=2.0, bw=10.0, bpt=125000, num_layers=16, w_tp=0.95, w_c=0.05, requests=make_reqs(200, mode='burst', seed=201))
    build_test(3, K=4, S=2.5, latency=3.0, bw=8.0, bpt=50000, num_layers=32, w_tp=0.05, w_c=0.95, requests=make_reqs(150, mode='streaming', seed=301))
    build_test(4, K=1, S=2.0, latency=2.0, bw=10.0, bpt=125000, num_layers=8, w_tp=0.5, w_c=0.5, requests=make_reqs(180, mode='streaming', seed=401))
    build_test(5, K=4, S=3.0, latency=4.0, bw=5.0, bpt=100000, num_layers=1, w_tp=0.7, w_c=0.3, requests=make_reqs(150, seed=501))
    build_test(6, K=4, S=10.0, latency=1.0, bw=20.0, bpt=50000, num_layers=8, w_tp=0.6, w_c=0.4, requests=make_reqs(120, seed=601))
    build_test(7, K=2, S=2.0, latency=30.0, bw=0.5, bpt=500000, num_layers=4, w_tp=0.4, w_c=0.6, requests=make_reqs(80, seed=701))
    build_test(8, K=8, S=1.0, latency=0.1, bw=50.0, bpt=25000, num_layers=64, w_tp=0.75, w_c=0.25, requests=make_reqs(300, seed=801))
    build_test(9, K=4, S=3.0, latency=5.0, bw=8.0, bpt=100000, num_layers=16, w_tp=0.6, w_c=0.4, requests=make_reqs(300, mode='burst', seed=901))
    build_test(10, K=2, S=2.0, latency=2.0, bw=10.0, bpt=100000, num_layers=8, w_tp=0.3, w_c=0.7, requests=make_reqs(100, mode='streaming', seed=1001))
    build_test(11, K=4, S=2.5, latency=3.0, bw=15.0, bpt=50000, num_layers=16, w_tp=0.5, w_c=0.5, requests=make_reqs(150, Lin_range=(2000, 4096), Lout_range=(1, 5), seed=1101))
    build_test(12, K=8, S=1.5, latency=2.0, bw=12.0, bpt=50000, num_layers=8, w_tp=0.85, w_c=0.15, requests=make_reqs(120, Lin_range=(10, 50), Lout_range=(100, 400), seed=1201))
    
    random.seed(1301)
    bimodal_reqs = []
    t = 0.0
    for i in range(200):
        t += random.uniform(5.0, 25.0)
        if i % 2 == 0: bimodal_reqs.append((t, 20, 2))
        else: bimodal_reqs.append((t, 3500, 150))
    build_test(13, K=4, S=3.0, latency=4.0, bw=10.0, bpt=100000, num_layers=16, w_tp=0.5, w_c=0.5, requests=bimodal_reqs)
    
    build_test(14, K=2, S=2.5, latency=3.5, bw=6.0, bpt=125000, num_layers=16, w_tp=0.5, w_c=0.5, requests=make_reqs(180, seed=1401))
    build_test(15, K=8, S=2.0, latency=2.5, bw=15.0, bpt=100000, num_layers=32, w_tp=0.85, w_c=0.15, requests=make_reqs(400, seed=1501))
    build_test(16, K=4, S=2.0, latency=2.0, bw=10.0, bpt=50000, num_layers=8, w_tp=0.2, w_c=0.8, requests=make_reqs(100, seed=1601), dist_base_override=0.0)
    build_test(17, K=8, S=1.0, latency=0.05, bw=100.0, bpt=125000, num_layers=16, w_tp=0.7, w_c=0.3, requests=make_reqs(350, seed=1701))
    build_test(18, K=2, S=3.0, latency=5.0, bw=8.0, bpt=1000000, num_layers=8, w_tp=0.5, w_c=0.5, requests=make_reqs(100, Lin_range=(10, 200), seed=1801))
    build_test(19, K=4, S=2.0, latency=3.0, bw=10.0, bpt=80000, num_layers=64, w_tp=0.4, w_c=0.6, requests=make_reqs(200, seed=1901))
    build_test(20, K=4, S=2.0, latency=2.0, bw=10.0, bpt=100000, num_layers=8, w_tp=0.5, w_c=0.5, requests=make_reqs(250, Lout_range=(1, 1), seed=2001))
    build_test(21, K=2, S=1.5, latency=1.5, bw=8.0, bpt=100000, num_layers=4, w_tp=0.5, w_c=0.5, requests=make_reqs(50, seed=2101))
    build_test(22, K=8, S=2.5, latency=3.0, bw=12.0, bpt=125000, num_layers=32, w_tp=0.5, w_c=0.5, requests=make_reqs(600, Lin_range=(20, 1500), Lout_range=(5, 50), seed=2201))

    # ═════════════════════════════════════════════════════════════════
    #  SUITE 2: Boundary & Extreme Regimes (Tests 23 to 35)
    # ═════════════════════════════════════════════════════════════════
    build_test(23, K=1, S=1.0, latency=0.001, bw=100.0, bpt=1000, num_layers=4, w_tp=0.5, w_c=0.5, requests=make_reqs(200, seed=2301))
    build_test(24, K=8, S=10.0, latency=50.0, bw=2.0, bpt=250000, num_layers=16, w_tp=0.5, w_c=0.5, requests=make_reqs(100, seed=2401))
    build_test(25, K=3, S=4.0, latency=5.0, bw=7.0, bpt=75000, num_layers=12, w_tp=0.6, w_c=0.4, requests=make_reqs(160, seed=2501))
    build_test(26, K=5, S=3.2, latency=2.8, bw=11.5, bpt=90000, num_layers=24, w_tp=0.35, w_c=0.65, requests=make_reqs(220, seed=2601))
    build_test(27, K=6, S=1.8, latency=1.2, bw=25.0, bpt=40000, num_layers=48, w_tp=0.8, w_c=0.2, requests=make_reqs(350, seed=2701))
    build_test(28, K=7, S=2.2, latency=4.1, bw=9.0, bpt=110000, num_layers=18, w_tp=0.45, w_c=0.55, requests=make_reqs(280, seed=2801))
    build_test(29, K=8, S=1.0, latency=1.0, bw=50.0, bpt=50000, num_layers=64, w_tp=0.99, w_c=0.01, requests=make_reqs(500, mode='burst', seed=2901))
    build_test(30, K=4, S=5.0, latency=10.0, bw=4.0, bpt=200000, num_layers=8, w_tp=0.01, w_c=0.99, requests=make_reqs(120, mode='streaming', seed=3001))
    build_test(31, K=2, S=8.0, latency=0.5, bw=40.0, bpt=30000, num_layers=2, w_tp=0.5, w_c=0.5, requests=make_reqs(150, seed=3101))
    build_test(32, K=8, S=2.0, latency=2.0, bw=20.0, bpt=100000, num_layers=32, w_tp=0.5, w_c=0.5, requests=make_reqs(300, Lin_range=(3500, 4096), Lout_range=(1, 5), seed=3201))
    build_test(33, K=8, S=2.0, latency=2.0, bw=20.0, bpt=100000, num_layers=32, w_tp=0.5, w_c=0.5, requests=make_reqs(100, Lin_range=(20, 50), Lout_range=(400, 512), seed=3301))
    build_test(34, K=1, S=10.0, latency=20.0, bw=1.0, bpt=500000, num_layers=1, w_tp=0.5, w_c=0.5, requests=make_reqs(60, seed=3401))
    build_test(35, K=8, S=1.0, latency=0.01, bw=100.0, bpt=10000, num_layers=64, w_tp=0.5, w_c=0.5, requests=make_reqs(800, mode='burst', seed=3501))

    # ═════════════════════════════════════════════════════════════════
    #  SUITE 3: Pathological Arrival & Heavy-Tail Topologies (Tests 36 to 50)
    # ═════════════════════════════════════════════════════════════════
    build_test(36, K=4, S=2.5, latency=3.0, bw=10.0, bpt=80000, num_layers=16, w_tp=0.5, w_c=0.5, requests=make_reqs(250, mode='periodic_pulse', seed=3601))
    build_test(37, K=8, S=2.0, latency=2.0, bw=15.0, bpt=100000, num_layers=32, w_tp=0.6, w_c=0.4, requests=make_reqs(350, mode='pareto_arrival', seed=3701))
    build_test(38, K=4, S=3.0, latency=4.0, bw=8.0, bpt=120000, num_layers=16, w_tp=0.4, w_c=0.6, requests=make_reqs(200, mode='pareto_len', seed=3801))
    build_test(39, K=2, S=1.5, latency=1.5, bw=12.0, bpt=60000, num_layers=8, w_tp=0.7, w_c=0.3, requests=make_reqs(180, mode='periodic_pulse', seed=3901))
    build_test(40, K=8, S=3.5, latency=6.0, bw=6.0, bpt=150000, num_layers=24, w_tp=0.5, w_c=0.5, requests=make_reqs(300, mode='pareto_len', seed=4001))
    build_test(41, K=3, S=2.0, latency=2.5, bw=10.0, bpt=80000, num_layers=12, w_tp=0.5, w_c=0.5, requests=make_reqs(150, mode='pareto_arrival', seed=4101))
    build_test(42, K=5, S=2.5, latency=3.0, bw=14.0, bpt=70000, num_layers=20, w_tp=0.6, w_c=0.4, requests=make_reqs(220, mode='periodic_pulse', seed=4201))
    build_test(43, K=7, S=1.8, latency=1.8, bw=18.0, bpt=50000, num_layers=28, w_tp=0.8, w_c=0.2, requests=make_reqs(400, mode='burst', seed=4301))
    build_test(44, K=4, S=4.5, latency=8.0, bw=5.0, bpt=300000, num_layers=16, w_tp=0.2, w_c=0.8, requests=make_reqs(120, mode='streaming', seed=4401))
    build_test(45, K=8, S=1.2, latency=0.2, bw=80.0, bpt=20000, num_layers=64, w_tp=0.5, w_c=0.5, requests=make_reqs(600, mode='pareto_len', seed=4501))
    build_test(46, K=1, S=3.0, latency=5.0, bw=8.0, bpt=100000, num_layers=16, w_tp=0.5, w_c=0.5, requests=make_reqs(150, mode='periodic_pulse', seed=4601))
    build_test(47, K=2, S=2.0, latency=2.0, bw=10.0, bpt=100000, num_layers=32, w_tp=0.5, w_c=0.5, requests=make_reqs(100, Lout_range=(1, 1), seed=4701))
    build_test(48, K=6, S=2.8, latency=3.5, bw=12.0, bpt=90000, num_layers=18, w_tp=0.5, w_c=0.5, requests=make_reqs(250, mode='pareto_arrival', seed=4801))
    build_test(49, K=4, S=1.5, latency=1.0, bw=30.0, bpt=40000, num_layers=8, w_tp=0.75, w_c=0.25, requests=make_reqs(300, mode='burst', seed=4901))
    build_test(50, K=8, S=3.0, latency=4.0, bw=10.0, bpt=125000, num_layers=32, w_tp=0.5, w_c=0.5, requests=make_reqs(750, mode='pareto_len', seed=5001))

    print("Successfully generated 50 comprehensive, highly diverse contest benchmark tests in local_tests/!")

if __name__ == "__main__":
    generate_generalized_suite()
