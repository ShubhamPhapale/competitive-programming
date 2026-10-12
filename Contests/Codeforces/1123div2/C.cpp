// Problem C
/**
 *    author:  shubhamphpefy
 *    created: 25.09.2026 20:35:21
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
#define d(x) cout<<(x)<<
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

template <typename T> bool mycomp(pair<T, T> x, pair<T, T> y) { return x.second < y.second; }

template <typename T>
T pow2 (T base, T exp) { T result = 1; while (exp > 0) { result = (exp % 2) ? result * base : result; base *= base; exp /= 2; } return result; }

vector<ll> tree;

ll segtree(int node, int node_low, int node_high, int query_low, int query_high)
{
    if(query_low <= node_low and node_high <= query_high) return tree[node];
    if(node_high < query_low or query_high < node_low) return 0;
    
    int last_in_left = (node_low + node_high) / 2;
    return segtree(2 * node, node_low, last_in_left, query_low, query_high) 
        + segtree(2 * node + 1, last_in_left + 1, node_high, query_low, query_high);
}

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
        ll x;
        cin >> n >> x;
        
        vector<ll> a(n);
        cin >> a;

        if (x == 1) { d1(0); continue; }

        vector<ll> d;
        for (ll i = 1; i * i <= x; ++i) {
            if (x % i == 0) {
                d.pb(i);
                if (i * i != x) d.pb(x / i);
            }
        }
        
        sor(d);

        int m = sz(d);
        vector<ll> sumg(m, 0);
        for (ll v : a) sumg[lwb(d, gcd(v, x))] += v;

        dbg(sumg);

        ll ans = 0;
        for (int i = 0; i < m; ++i) {
            if (d[i] == 1) continue;
            
            ll cur = 0;
            for (int j = i; j < m; j++) if (d[j] % d[i] == 0) cur += sumg[j];
            
            ans = max(ans, cur);
        }
        
        d1(ans);
    }
    
    return 0;
}