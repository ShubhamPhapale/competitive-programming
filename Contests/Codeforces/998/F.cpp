// Problem F
/**
 *    author:  shubhamphpefy
 *    created: 25.02.2025 17:54:22
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
const int mod=998244353; //1e9+7;
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

const int K_MAX = 100000;  
// 2^16 < 1e5
const int MAX_R = 18;
 
ll pow2(ll base, ll exp, ll m = mod) {
    ll res = 1 % m;
    base %= m;
    while(exp > 0){
        if(exp & 1) res = (res * base) % m;
        base = (base * base) % m;
        exp >>= 1;
    }
    return res;
}
 
const int MAXF = MAX_R + 2;
vector<ll> fact(MAXF), invfact(MAXF);
 
void precomputeFactorials(){
    fact[0] = 1;
    for (int i = 1; i < MAXF; i++){
        fact[i] = (fact[i-1] * i) % mod;
    }
    for (int i = 0; i < MAXF; i++){
        invfact[i] = pow2(fact[i], mod-2, mod);
    }
}
 
ll choose_n_r(ll n, int r){ 
    ll num = 1;
    for (int i = 0; i <= r; i++){
        num = (num * ((n + 1 - i) % mod)) % mod;
    }
    ll den = fact[r+1];
    ll invDen = pow2(den, mod-2, mod);
    return (num * invDen) % mod;
}
 
// dp[x][r] = number of ordered sequences of factors (>1) 
// that their product x, 0 <= r < MAX_R
// dp[1][0] = 1; for x > 1, dp[x][r] = sum_{d divides x, d>=2} dp[x/d][r-1]

vector<vector<int>> dpArr(K_MAX + 1, vector<int>(MAX_R, 0));
 
vector<vector<int>> divisorsArr(K_MAX + 1);
 
void precomputeDivisors(){ 
    for (int d = 2; d <= K_MAX; d++){
        for (int mult = d; mult <= K_MAX; mult += d){
            divisorsArr[mult].pb(d);
        }
    }
}
 
// For x=1 
// For x >= 2 Any valid array has some non-1 factors
// C(n+1, r+1)
// sum_{r=1}^{MAX_R-1} (dpArr[x][r] * C(n+1, r+1)) % mod
 
void precomputeDP(){
    dpArr[1][0] = 1; 
    for (int r = 1; r < MAX_R; r++){
        dpArr[1][r] = 0;
    }
    for (int x = 2; x <= K_MAX; x++){
        for (int r = 1; r < MAX_R; r++){
            int ways = 0;
            for (int d : divisorsArr[x]){
                int y = x / d;
                ways = (ways + dpArr[y][r - 1]) % mod;
            }
            dpArr[x][r] = ways;
        }
        // dbg(dpArr);
    }
}
 
struct Input {
    int k; 
    ll n;
};
 
int main(){
    lg();
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(12);
    
    precomputeFactorials();
    precomputeDivisors();
    precomputeDP();
    
    int tcs = 1;
    cin >> tcs;
    while(tcs--){
        int k; 
        ll n;
        cin >> k >> n;
        
        vector<int> ans(k + 1, 0);
        ans[1] = n % mod;
        
        vector<ll> comb(MAX_R, 0);
        for (int r = 1; r < MAX_R; r++){
            comb[r] = choose_n_r(n, r);
        }
        
        for (int x = 2; x <= k; x++){
            ll ways = 0;
            for (int r = 1; r < MAX_R; r++){
                if(dpArr[x][r]){
                    dbg(ways);
                    ways = (ways + ( (ll)dpArr[x][r] * comb[r] ) % mod) % mod;
                }
            }
            ans[x] = ways % mod;
        }
        
        for (int x = 1; x <= k; x++){
            cout << ans[x] << (x == k ? "\n" : " ");
        }
    }
    return 0;
}

// stress slow F 1000000