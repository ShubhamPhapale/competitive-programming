/**
 *    author:  shubhamphpefy
 *    created: 17.11.2024 01:31:43
**/

// solution for https://open.kattis.com/contests/nasouth24d1open/problems/intergalacticteam

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
#include "algo/debug.h"
#include "algo/log.h"
#else
#define dbg(...) 42
#define lg(...) 42
#endif

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

void solve()
{
    int n, m, k;
    cin>>n>>m>>k;
    
    set<pair<int, int>> edges;
    
    for(int i=0;i<m;i++)
    {
        int x, y;
        cin>>x>>y;
        
        x--, y--;
        edges.insert({x, y});
    }
    
    vector<vector<int>> graph(n);
    bool ok[n];
    for(int i=0;i<n;i++) ok[i] = true;
    for(auto a : edges){
        if(a.f < a.s and edges.count({a.s, a.f})){
            graph[a.f].pb(a.s);
            graph[a.s].pb(a.f);
        }
        if(!edges.count({a.s, a.f})){
            ok[a.s] = false;
            ok[a.f] = false;
        }
    }
    
    int ans = 0;
    
    for(int i=0;i<n;i++)
    {
        if(sz(graph[i]) == k-1 and ok[i])
        {
            for(auto a : graph[i])
            {
                if(a < i) goto done;
                if(sz(graph[a]) != k-1 or !ok[a]) goto done;
            }
            
            set<int> neighbours;
            for(auto a : graph[i]){
                neighbours.insert(a);
            }
            
            neighbours.insert(i);
            for(auto a : graph[i]){
                for(auto b : graph[a]){
                    if(!neighbours.count(b)){
                        goto done;
                    }
                }
            }
            ans++;
        }
        
        done:
        ;
    }
    cout<<ans<<endl;
    
}

int main() {
    lg();
    
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(12);
    
    int tcs = 1;
    // cin >> tcs;
    for(int tc = 0; tc < tcs; ++tc) 
    {
        solve();
    }
    
    return 0;
}
