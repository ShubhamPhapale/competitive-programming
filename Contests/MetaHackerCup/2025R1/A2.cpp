// Problem A2

/**
 *    author:  shubhamphpefy
 *    created: 18.10.2025 22:54:14
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

struct DSU {
    int n;
    vector<int> p, r;
    int comps;
    
    DSU(int n = 0): n(n), p(n), r(n,0), comps(n) {
        iota(all(p), 0);
    }
    
    int find(int x){
        return p[x] == x ? x : p[x] = find(p[x]);
    }
    
    bool unite(int a, int b){
        a = find(a); b = find(b);
        
        if(a == b) return false;
        if(r[a] < r[b]) swap(a,b);
        
        p[b] = a;
        
        if(r[a] == r[b]) r[a]++;
        comps--;
        
        return true;
    }
};

struct Edge {
    ll w;
    int u, v;
};

int main() {
	lg();
	
	ios_base::sync_with_stdio(false);
	cin.tie(nullptr);
	cout << fixed << setprecision(12);
	
	int tcs = 1;
	cin >> tcs;
	for(int tc = 0; tc < tcs; ++tc) 
	{
		int n;
	    cin >> n;
	    vector<ll> v(n + 1);
	    for(int i = 1; i <= n; ++i) cin >> v[i];

	    vector<Edge> es;
	    es.reserve(2 * n);
	    
	    for(int i = 1; i <= n; ++i){
	        es.pb({v[i], 0, i});
	    }
	    
	    for(int i = 1; i <= n - 1; ++i){
	        es.pb({ llabs(v[i] - v[i + 1]), i, i + 1 });
	    }

	    sort(all(es), [](Edge &a, Edge &b){
	        return a.w < b.w;
	    });

	    DSU dsu(n + 1);
	    ll ans = 0;
	    
	    for(auto &e : es){
	        if(dsu.unite(e.u, e.v)){
	            ans = max(ans, e.w);
	            if(dsu.comps == 1) break;
	        }
	    }

	    cout << "Case #" << (tc+1) << ": " << ans << "\n";
	}
	
	return 0;
}