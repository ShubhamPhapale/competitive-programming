#include <bits/stdc++.h>

// ----- Segment Tree (Point Update, Range Query) -----
template <typename T>
struct SegTree {
    int n;
    std::vector<T> tree;
    T identity;

    T merge(T left, T right) {
        return left + right; // Customize: std::min(left, right), std::max, std::gcd, etc.
    }

    SegTree(std::vector<T> v, T id) {
        identity = id;
        n = int(v.size());
        while (__builtin_popcount(n) != 1) {
            v.push_back(identity);
            n++;
        }
        
        tree.assign(2 * n, identity);
        
        for (int i = 0; i < n; i++) tree[n + i] = v[i];
        for (int i = n - 1; i > 0; i--) tree[i] = merge(tree[2 * i], tree[2 * i + 1]);
    }

    T segtree(int node, int node_low, int node_high, int query_low, int query_high, T value, bool is_update) {
        if (query_low <= node_low && node_high <= query_high) {
            if (is_update) tree[node] = value;
            return tree[node];
        }
        if (node_high < query_low || query_high < node_low) return identity;
        
        int last_in_left = (node_low + node_high) / 2;
        
        T ans = merge(
            segtree(2 * node, node_low, last_in_left, query_low, query_high, value, is_update),
            segtree(2 * node + 1, last_in_left + 1, node_high, query_low, query_high, value, is_update)
        );
        
        tree[node] = merge(tree[2 * node], tree[2 * node + 1]);
        return ans;
    }

    void update(int i, T value) {
        segtree(1, 0, n - 1, i, i, value, true);
    }

    T query(int low, int high) {
        return segtree(1, 0, n - 1, low, high, identity, false);
    }
};