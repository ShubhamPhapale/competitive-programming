#include <bits/stdc++.h>
using namespace std;

// ----- Sparse Table (Static Range Queries: min, max, gcd, etc.) -----
template <typename T>
struct SparseTable {
    int n;
    vector<vector<T>> sp;

    // Must be idempotent operation: min/max/gcd/AND/OR — NOT sum
    T combine(T x, T y) { return std::gcd(x, y); }

    SparseTable(vector<T> v) {
        n = int(v.size());
        int lvl = 1;
        while ((1 << lvl) <= n) lvl++;
        sp.assign(lvl, vector<T>(n));
        sp[0] = v;
        for (int j = 1; j < lvl; j++)
            for (int i = 0; i + (1 << j) <= n; i++)
                sp[j][i] = combine(sp[j - 1][i], sp[j - 1][i + (1 << (j - 1))]);
    }

    // Query [l, r] (0-indexed inclusive)
    T query(int l, int r) {
        int k = 31 - __builtin_clz(r - l + 1);
        return combine(sp[k][l], sp[k][r + 1 - (1 << k)]);
    }
};
