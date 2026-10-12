// Problem B
/**
 *    author:  shubhamphpefy
 *    created: 30.09.2026 20:23:35
**/
#include <algorithm>
#include <array>
#include <bitset>
#include <cassert>
#include <chrono>
#include <climits>
#include <cmath>
#include <complex>
#include <cstring>
#include <functional>
#include <iomanip>
#include <iostream>
#include <map>
#include <numeric>
#include <queue>
#include <random>
#include <set>
#include <vector>
#include <stack>

using namespace std;

#ifdef LOCAL
#include "debug.h"
#include "log.h"
#else
#define dbg(...) 42
#define lg(...) 42
#endif

#define uid(a, b) uniform_int_distribution<int>(a, b)(rnd)
 
// mt19937 rnd(239);
mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

#define ll long long
#define ld long double
#define mp make_pair
#define f first
#define s second

#define yes cout << "YES" << endl
#define no cout << "NO" << endl

#define d0(x) cout<<(x)<<" "
#define d1(x) cout<<(x)<<endl
#define d2(x,y) cout<<(x)<<" "<<(y)<<endl
#define d3(x,y,z) cout<<(x)<<" "<<(y)<<" "<<(z)<<endl
#define d4(a,b,c,d) cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<endl
#define d5(a,b,c,d,e) cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<" "<<(e)<<endl
#define d6(a,b,c,d,e,f) cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<" "<<(e)<<" "<<(f)<<endl

#define sz(x) int((x).size())
#define bg(x) begin(x)
#define all(x) bg(x), end(x)
#define rall(x) x.rbegin(), x.rend()
#define sor(x) sort(all(x))
#define rsz resize
#define ins insert
#define pb push_back
#define eb emplace_back
#define ft front()
#define bk back()
 
#define lb lower_bound
#define ub upper_bound
template <class T> int lwb(vector<T> &a, const T &b) { return int(lb(all(a), b) - a.begin()); }
template <class T> int upb(vector<T> &a, const T &b) { return int(ub(all(a), b) - a.begin()); }

const int maxn=1e6+100;
const int mod=1e9+7;
const int INF=1e9;
const ll LLINF=1e18;
const double EPS = 1e-9;
const double PI=3.14159265358979323846264338327950288419716939937510582097494459230;

template<class T, class U> istream& operator>>(istream &is, pair<T, U>& p) { is >> p.first >> p.second; return is; }
template<class T> istream& operator>>(istream& is, vector<T>& vec) { for(auto &x : vec) cin >> x; return is; } 
template<class T, class U> ostream& operator<<(ostream &os, const pair<T, U>& p) { os << "<" << p.first << "," << p.second << ">"; return os; }
template<class T> ostream& operator<<(ostream& os, const vector<T>& vec) { for(auto x : vec) os << x << " "; return os; }

template <typename T>
struct SegTree {
    int n;
    vector<T> tree;
    T identity;

    T merge(T left, T right) {
        return left + right; 
    }

    SegTree(vector<T> v, T id) {
        identity = id;
        n = sz(v);
        while(__builtin_popcount(n) != 1){
            v.pb(identity); 
            n++;
        }
        
        tree.assign(2 * n, identity);
        
        for(int i = 0; i < n; i++) tree[n + i] = v[i];
        for(int i = n - 1; i > 0; i--) tree[i] = merge(tree[2 * i], tree[2 * i + 1]);
    }

    T segtree(int node, int node_low, int node_high, int query_low, int query_high, T value, bool is_update) {
        if(query_low <= node_low and node_high <= query_high){
            if(is_update) tree[node] = value;
            return tree[node];
        }
        if(node_high < query_low or query_high < node_low) return identity;
        
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

template <typename T>
struct Fenwick {
    int n;
    vector<T> tree;

    Fenwick(int n) : n(n), tree(n + 1, 0) {}

    void update(int i, T delta) {
        for (++i; i <= n; i += i & (-i)) tree[i] += delta;
    }

    T query(int i) {
        T sum = 0;
        for (++i; i > 0; i -= i & (-i)) sum += tree[i];
        return sum;
    }

    T query(int l, int r) { return query(r) - (l ? query(l - 1) : 0); }
};

template <typename T>
struct SparseTable {
    int n;
    vector<vector<T>> sp;

    T combine(T x, T y) { return gcd(x, y); } // must be idempotent: min/max/gcd/AND/OR — NOT sum

    SparseTable(vector<T> v) {
        n = sz(v);
        int lvl = 1;
        while ((1 << lvl) <= n) lvl++;
        sp.assign(lvl, vector<T>(n));
        sp[0] = v;
        for (int j = 1; j < lvl; j++)
            for (int i = 0; i + (1 << j) <= n; i++)
                sp[j][i] = combine(sp[j - 1][i], sp[j - 1][i + (1 << (j - 1))]);
    }

    T query(int l, int r) {
        int k = 31 - __builtin_clz(r - l + 1);
        return combine(sp[k][l], sp[k][r + 1 - (1 << k)]);
    }
};

template <typename T>
struct DSU {
    vector<T> par;
    vector<vector<T>> members;

    DSU(T n) {
        par.rsz(n + 1);
        iota(all(par), 0);
        members.rsz(n + 1);
        for(T i = 0; i <= n; i++) members[i] = {i};
    }

    T find(T x) {
        return par[x] == x ? x : par[x] = find(par[x]);
    }

    bool unite(T x, T y) {
        x = find(x), y = find(y);
        if (x == y) return false;

        if (sz(members[x]) < sz(members[y])) swap(x, y);
        
        par[y] = x;
        
        members[x].ins(members[x].end(), all(members[y]));
        members[y].clear(); 
        members[y].shrink_to_fit(); 
        
        return true;
    }
};

template <typename T>
struct DSURollback {
    vector<T> par, siz;
    T components;
    vector<pair<T, T>> history;

    DSURollback(T n) {
        components = n;
        par.rsz(n + 1);
        iota(all(par), 0);
        siz.assign(n + 1, 1);
    }

    T find(T x) {
        return par[x] == x ? x : find(par[x]);
    }

    bool unite(T x, T y) {
        x = find(x), y = find(y);
        if (x == y) {
            history.eb(-1, -1);
            return false;
        }

        if (siz[x] < siz[y]) swap(x, y);
        
        par[y] = x;
        siz[x] += siz[y];
        components--;
        
        history.eb(y, x);
        return true;
    }

    void rollback() {
        if (sz(history) == 0) return;
        
        auto [y, x] = history.bk;
        history.pop_back();
        
        if (y != -1) {
            siz[x] -= siz[y];
            par[y] = y;
            components++;
        }
    }
};

template <typename T>
struct Dijkstra {
    int n;
    vector<vector<pair<int, T>>> adj;
    vector<T> dist;
    vector<int> par;
    const T INF_VAL;

    Dijkstra(int n, T inf_val = LLINF) : n(n), INF_VAL(inf_val) {
        adj.rsz(n + 1);
        dist.assign(n + 1, INF_VAL);
        par.assign(n + 1, -1);
    }

    void add_edge(int u, int v, T w, bool directed = false) {
        adj[u].pb({v, w});
        if (!directed) adj[v].pb({u, w});
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
                int v = edge.f;
                T w = edge.s;
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
        for (int v = target; v != -1; v = par[v]) path.pb(v);
        reverse(all(path));
        return path;
    }
};

template <typename T>
struct LCA {
    int n, l, timer;
    vector<vector<pair<int, T>>> adj;
    vector<int> tin, tout, depth;
    vector<vector<int>> up;
    vector<T> dist;

    LCA(int n) : n(n), timer(0) {
        adj.rsz(n + 1);
        tin.rsz(n + 1);
        tout.rsz(n + 1);
        depth.assign(n + 1, 0);
        dist.assign(n + 1, 0);
        l = ceil(log2(n + 1));
        up.assign(n + 1, vector<int>(l + 1));
    }

    void add_edge(int u, int v, T w = 1) {
        adj[u].pb({v, w});
        adj[v].pb({u, w}); 
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
            int u = edge.f;
            T w = edge.s;
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

template <int MOD>
struct ModInt {
    ll val;
    ModInt(ll v = 0) { val = v % MOD; if (val < 0) val += MOD; }
    ModInt operator+(const ModInt &o) const { return ModInt(val + o.val); }
    ModInt operator-(const ModInt &o) const { return ModInt(val - o.val); }
    ModInt operator*(const ModInt &o) const { return ModInt(val * o.val % MOD); }
    ModInt pow(ll e) const { ModInt res(1), b(*this); while (e) { if (e & 1) res = res * b; b = b * b; e >>= 1; } return res; }
    ModInt inv() const { return pow(MOD - 2); }
    ModInt operator/(const ModInt &o) const { return *this * o.inv(); }
    ModInt& operator+=(const ModInt &o) { return *this = *this + o; }
    ModInt& operator-=(const ModInt &o) { return *this = *this - o; }
    ModInt& operator*=(const ModInt &o) { return *this = *this * o; }
    friend ostream& operator<<(ostream &os, const ModInt &m) { return os << m.val; }
};
using mint = ModInt<mod>;

vector<mint> fact, inv_fact;
void precomp_fact(int n) {
    fact.rsz(n + 1); 
    inv_fact.rsz(n + 1);
    fact[0] = 1;
    for (int i = 1; i <= n; i++) fact[i] = fact[i - 1] * i;
    inv_fact[n] = fact[n].inv();
    for (int i = n; i > 0; i--) inv_fact[i - 1] = inv_fact[i] * i;
}

mint nCr(int n, int r) {
    if (r < 0 || r > n) return mint(0);
    return fact[n] * inv_fact[r] * inv_fact[n - r];
}

vector<int> spf;
void sieve(int n) {
    spf.assign(n + 1, 0);
    for (int i = 2; i <= n; i++) {
        if (!spf[i]) for (int j = i; j <= n; j += i) if (!spf[j]) spf[j] = i;
    }
}

template <typename T> bool mycomp(pair<T, T> x, pair<T, T> y) { return x.second < y.second; }

int main() {
    lg();
    
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr); // comment out for interactive problems
    cout << fixed << setprecision(12);
    
    int tcs = 1;
    cin >> tcs;
    for(int tc = 0; tc < tcs; ++tc) 
    {
        int n;
        cin >> n;
        vector<int> a(n);
        cin >> a;
        
        vector<int> cnt(n + 105, 0);
        for(int x : a) if (x < n + 105) cnt[x]++;
        
        int mex = 0;
        while(cnt[mex] > 0) mex++;
        
        ll moves = 0;
        for(int x : a) {
            if(x < mex) moves += x;  
            else if (x > mex) moves += (x - mex - 1);
        }
        
        moves -= 1ll * mex * (mex - 1) / 2;
        
        if (moves % 2 != 0) d1("Alice");
        else d1("Bob");
    }
    
    return 0;
}