#include <bits/stdc++.h>
using namespace std;

// ----- Smallest Prime Factor (SPF) Sieve & Factorization -----
vector<int> spf;

void sieve(int n) {
    spf.assign(n + 1, 0);
    for (int i = 2; i <= n; i++) {
        if (!spf[i]) {
            for (int j = i; j <= n; j += i) {
                if (!spf[j]) spf[j] = i;
            }
        }
    }
}

// O(log x) factorization using precomputed SPF
vector<pair<int, int>> get_factors(int x) {
    vector<pair<int, int>> factors;
    while (x > 1) {
        int p = spf[x], count = 0;
        while (x % p == 0) {
            count++;
            x /= p;
        }
        factors.push_back({p, count});
    }
    return factors;
}