#include <bits/stdc++.h>

// ----- Disjoint Set Union (DSU / Union-Find) -----
template <typename T = int>
struct DSU {
    std::vector<T> par;
    std::vector<std::vector<T>> members;

    DSU(T n) {
        par.resize(n + 1);
        std::iota(par.begin(), par.end(), 0);
        members.resize(n + 1);
        for (T i = 0; i <= n; i++) members[i] = {i};
    }

    T find(T x) {
        return par[x] == x ? x : par[x] = find(par[x]);
    }

    bool unite(T x, T y) {
        x = find(x), y = find(y);
        if (x == y) return false;

        if (members[x].size() < members[y].size()) std::swap(x, y);
        par[y] = x;
        members[x].insert(members[x].end(), members[y].begin(), members[y].end());
        members[y].clear();
        members[y].shrink_to_fit();
        return true;
    }
};

// ----- DSU with Rollback Support -----
template <typename T = int>
struct DSURollback {
    std::vector<T> par, siz;
    T components;
    std::vector<std::pair<T, T>> history;

    DSURollback(T n) {
        components = n;
        par.resize(n + 1);
        std::iota(par.begin(), par.end(), 0);
        siz.assign(n + 1, 1);
    }

    T find(T x) {
        return par[x] == x ? x : find(par[x]);
    }

    bool unite(T x, T y) {
        x = find(x), y = find(y);
        if (x == y) {
            history.emplace_back(-1, -1);
            return false;
        }

        if (siz[x] < siz[y]) std::swap(x, y);
        par[y] = x;
        siz[x] += siz[y];
        components--;

        history.emplace_back(y, x);
        return true;
    }

    void rollback() {
        if (history.empty()) return;
        auto [y, x] = history.back();
        history.pop_back();

        if (y != -1) {
            siz[x] -= siz[y];
            par[y] = y;
            components++;
        }
    }
};
