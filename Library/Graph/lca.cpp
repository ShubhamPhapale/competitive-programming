#include <bits/stdc++.h>

// ----- Lowest Common Ancestor (Binary Lifting) -----
template <typename T = long long>
struct LCA {
    int n, l, timer;
    std::vector<std::vector<std::pair<int, T>>> adj;
    std::vector<int> tin, tout, depth;
    std::vector<std::vector<int>> up;
    std::vector<T> dist;

    LCA(int n) : n(n), timer(0) {
        adj.resize(n + 1);
        tin.resize(n + 1);
        tout.resize(n + 1);
        depth.assign(n + 1, 0);
        dist.assign(n + 1, 0);
        l = std::ceil(std::log2(n + 1));
        up.assign(n + 1, std::vector<int>(l + 1));
    }

    void add_edge(int u, int v, T w = 1) {
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }

    void dfs(int v, int p, T d, int dpth) {
        tin[v] = ++timer;
        up[v][0] = p;
        dist[v] = d;
        depth[v] = dpth;

        for (int i = 1; i <= l; ++i) {
            up[v][i] = up[up[v][i - 1]][i - 1];
        }

        for (auto& edge : adj[v]) {
            int u = edge.first;
            T w = edge.second;
            if (u != p) dfs(u, v, d + w, dpth + 1);
        }
        tout[v] = ++timer;
    }

    void build(int root = 1) {
        dfs(root, root, 0, 0);
    }

    bool is_ancestor(int u, int v) {
        return tin[u] <= tin[v] && tout[u] >= tout[v];
    }

    int get_lca(int u, int v) {
        if (is_ancestor(u, v)) return u;
        if (is_ancestor(v, u)) return v;
        for (int i = l; i >= 0; --i) {
            if (!is_ancestor(up[u][i], v)) u = up[u][i];
        }
        return up[u][0];
    }

    T get_dist(int u, int v) {
        return dist[u] + dist[v] - 2 * dist[get_lca(u, v)];
    }

    int kth_ancestor(int u, int k) {
        if (depth[u] < k) return -1;
        for (int i = 0; i <= l; i++) {
            if ((k >> i) & 1) u = up[u][i];
        }
        return u;
    }
};
