// Problem C2
/**
 *    author:  shubhamphpefy
 *    created: 12.09.2026 20:34:57
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
        
        vector<ll> a(n + 1);
        for (int i = 1; i <= n; i++) cin >> a[i];

        vector<int> diff(n + 2, 0);
        for (int i = 1; i <= n; i++) {
            ll l = a[i] * i;
            ll r = (a[i] + 1) * i - 1;
            
            if (l < n) {
                diff[l]++;
                diff[min((ll)n, r + 1)]--;
            }
        }

        vector<int> p = {-1};
        int cur = 0;
        for (int i = 0; i < n; i++) {
            cur += diff[i];
            if (cur == 0) p.pb(i);
        }

        int m = sz(p) - 1;
        vector<int> prv(n + 1, 0), nxt(n + 1, m + 1);
        
        for (int i = 1; i <= m; i++) prv[p[i]] = nxt[p[i]] = i;
        for (int i = 1; i <= n; i++) if (!prv[i]) prv[i] = prv[i - 1];
        for (int i = n - 1; i >= 0; i--) if (nxt[i] == m + 1) nxt[i] = nxt[i + 1];
        
        dbg(prv, nxt);
    
        bool ok = true;
        vector<int> maxl(m + 2, 0);
        for (int k = 1; k <= n; k++) {
            for (int v = 0; v < a[k]; v++) {
                ll l = 1ll * v * k; 
                if (l >= n) break;
                ll r = min((ll)n - 1, 1ll * (v + 1) * k - 1);
                
                int lll = nxt[l], rr = prv[r];
                dbg(lll, rr);
                if (l > r) { ok = false; break; }
                maxl[rr + 1] = max(maxl[rr + 1], lll);
            }
            if (!ok) break;
        }
        
        dbg(maxl);
        
        if (!ok) { d1("0"); continue; }
        
        for (int i = 1; i <= m + 1; i++) maxl[i] = max(maxl[i], maxl[i - 1]);

        vector<int> dp(m + 2, 0), sum(m + 2, 0);
        dp[0] = sum[0] = 1;
        
        for (int i = 1; i <= m + 1; i++) {
            int l = maxl[i];
            dp[i] = (sum[i - 1] - (l > 0 ? sum[l - 1] : 0) + mod) % mod;
            sum[i] = (sum[i - 1] + dp[i]) % mod;
        }

        d1(dp[m + 1]);
    }
    
    return 0;
}