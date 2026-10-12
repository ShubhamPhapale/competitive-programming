// Problem c
/**
 *    author:  shubhamphpefy
 *    created: 21.04.2025 20:18:21
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

#ifdef LOcAL
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
const double EPs = 1e-9;
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
    cin.tie(nullptr);
    cout << fixed << setprecision(12);
    
    int tcs = 1;
    cin >> tcs;
    for(int tc = 0; tc < tcs; ++tc) 
    {
        int n; 
        ll k;
        cin >> n >> k;
        
        vector<ll> a(n);
        cin >> a;

        vector<int> b(n);
        for(int i = 0; i < n; i++) b[i] = (a[i] <= k ? 1 : -1);

        vector<ll> s(n + 1, 0);
        for(int i = 1; i <= n; i++) s[i] = s[i - 1] + b[i - 1];
            
        vector<char> pre(n + 1, 0), sf(n + 1, 0);
        for(int i = 1; i <= n - 1; i++){
            ll t1 = (i % 2 ? 1 : 0);
            if(s[i] >= t1) pre[i] = 1;
        }
        
        for(int r = 1; r <= n - 1; r++){
            ll tt = ((n - r) % 2 ? 1 : 0);
            if(s[n] - s[r] >= tt) sf[r] = 1;
        }

        int mnl = n + 1, mxr = -1;
        for(int i = 1;i <= n - 1; i++){
            if(pre[i]) mnl = min(mnl, i);
            if(sf[i]) mxr = max(mxr, i);
        }
        
        if(mnl < mxr){
            yes;
            continue;
        }

        vector<ll> a0(n + 2, LLONG_MIN), a1(n + 2, LLONG_MIN), b0(n + 2, LLONG_MIN), b1(n + 2, LLONG_MIN);
        for(int i = n; i >= 1; i--){
            a0[i] = a0[i + 1];
            a1[i] = a1[i + 1];
            b0[i] = b0[i + 1];
            b1[i] = b1[i + 1];
            
            if(i <= n - 1){
                if(i % 2 == 0) a0[i] = max(a0[i], s[i]);
                else a1[i] = max(a1[i], s[i]);
                if(i % 2 == 0) b0[i] = max(b0[i], s[i] - 1);
                else b1[i] = max(b1[i], s[i] - 1);
                lg("here");
            }
        }

        bool ok = false;
        for(int l = 1; l <= n - 2; l++){
            if(!pre[l]) continue;
            
            ll sl = s[l];
            if(l % 2 == 0) if(a0[l + 1] >= sl || b1[l + 1] >= sl) { ok = true; lg("here1"); break; }
            else if(a1[l + 1] >= sl || b0[l + 1] >= sl) { ok = true; break; }
        }
        
        if(ok){
            yes;
            continue;
        }

        vector<ll> c0(n + 1, LLONG_MAX), c1(n + 1, LLONG_MAX);
        for(int i = 1; i <= n; i++){
            c0[i] = c0[i - 1];
            c1[i] = c1[i - 1];
            
            if(i % 2 == 0) c0[i] = min(c0[i], s[i]);
            else c1[i] = min(c1[i], s[i]);
        }

        for(int r = 2;r <= n - 1; r++){
            if(!sf[r]) continue;
            ll sr = s[r];
            dbg(sr);
            if(r % 2 == 0) if(c0[r - 1] <= sr || c1[r - 1] <= sr - 1) { ok = true; break; }
            else if(c1[r - 1] <= sr || c0[r - 1] <= sr - 1) { ok = true; break; }
        }

        ok ? yes : no;
    }
    
    return 0;
}