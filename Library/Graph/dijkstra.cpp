#include <bits/stdc++.h>
using namespace std;

// ----- Dijkstra's Algorithm (Single Source Shortest Path with Path Reconstruction) -----
template <typename T = long long>
struct Dijkstra {
    int n;
    vector<vector<pair<int, T>>> adj;
    vector<T> dist;
    vector<int> par;
    const T INF_VAL;

    Dijkstra(int n, T inf_val = 1e18) : n(n), INF_VAL(inf_val) {
        adj.resize(n + 1);
        dist.assign(n + 1, INF_VAL);
        par.assign(n + 1, -1);
    }

    void add_edge(int u, int v, T w, bool directed = false) {
        adj[u].push_back({v, w});
        if (!directed) adj[v].push_back({u, w});
    }

    void run(int src) {
        dist.assign(n + 1, INF_VAL);
        par.assign(n + 1, -1);
        priority_queue<pair<T, int>, vector<pair<T, int>>, greater<pair<T, int>>> pq;

        dist[src] = 0;
        pq.push({0, src});

        while (!pq.empty()) {
            auto [d, u] = pq.top();
            pq.pop();

            if (d > dist[u]) continue;

            for (auto& edge : adj[u]) {
                int v = edge.first;
                T w = edge.second;
                if (dist[u] + w < dist[v]) {
                    dist[v] = dist[u] + w;
                    par[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }
    }

    vector<int> restore_path(int target) {
        if (dist[target] == INF_VAL) return {};
        vector<int> path;
        for (int v = target; v != -1; v = par[v]) path.push_back(v);
        reverse(path.begin(), path.end());
        return path;
    }
};