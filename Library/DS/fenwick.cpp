#include <bits/stdc++.h>
using namespace std;

// ----- Fenwick Tree (Binary Indexed Tree) -----
template <typename T>
struct Fenwick {
    int n;
    vector<T> tree;

    Fenwick(int n) : n(n), tree(n + 1, 0) {}

    // Point update: add delta to index i (0-indexed)
    void update(int i, T delta) {
        for (++i; i <= n; i += i & (-i)) tree[i] += delta;
    }

    // Prefix sum query [0, i] (0-indexed)
    T query(int i) {
        T sum = 0;
        for (++i; i > 0; i -= i & (-i)) sum += tree[i];
        return sum;
    }

    // Range sum query [l, r] (0-indexed)
    T query(int l, int r) {
        return query(r) - (l ? query(l - 1) : 0);
    }
};