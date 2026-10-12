/**
 *    author:  shubhamphpefy
 *    created: 13.10.2026 02:46:07
**/
#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace __gnu_pbds;

template <typename T>
using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag, tree_order_statistics_node_update>;


#ifdef LOCAL
#include "debug.h"
#include "log.h"
#else
#define dbg(...) 42
#define lg(...) 42
#endif

#define uid(a, b) std::uniform_int_distribution<int>(a, b)(rnd)
 
// std::mt19937 rnd(239);
std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());

#define ll long long
#define ld long double
#define mp std::make_pair
#define f first
#define s second

#define yes std::cout << "YES" << std::endl
#define no std::cout << "NO" << std::endl

#define d0(x) std::cout<<(x)<<" "
#define d1(x) std::cout<<(x)<<std::endl
#define d2(x,y) std::cout<<(x)<<" "<<(y)<<std::endl
#define d3(x,y,z) std::cout<<(x)<<" "<<(y)<<" "<<(z)<<std::endl
#define d4(a,b,c,d) std::cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<std::endl
#define d5(a,b,c,d,e) std::cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<" "<<(e)<<std::endl
#define d6(a,b,c,d,e,f) std::cout<<(a)<<" "<<(b)<<" "<<(c)<<" "<<(d)<<" "<<(e)<<" "<<(f)<<std::endl

#define sz(x) int((x).size())
#define bg(x) std::begin(x)
#define all(x) bg(x), std::end(x)
#define rall(x) x.rbegin(), x.rend()
#define sor(x) std::sort(all(x))
#define rsz resize
#define ins insert
#define pb push_back
#define eb emplace_back
#define ft front()
#define bk back()
 
#define lb std::lower_bound
#define ub std::upper_bound
template <class T> int lwb(std::vector<T> &a, const T &b) { return int(lb(all(a), b) - a.begin()); }
template <class T> int upb(std::vector<T> &a, const T &b) { return int(ub(all(a), b) - a.begin()); }

const int MAXN = 1e6 + 100;
const int MOD = 1e9 + 7;
const int MOD2 = 998244353;
const int INF = 1e9;
const ll LLINF = 1e18;
const double EPS = 1e-9;
const double PI = 3.14159265358979323846264338327950288419716939937510582097494459230;

template<class T, class U> std::istream& operator>>(std::istream &is, std::pair<T, U>& p) { is >> p.first >> p.second; return is; }
template<class T> std::istream& operator>>(std::istream& is, std::vector<T>& vec) { for(auto &x : vec) is >> x; return is; } 
template<class T, class U> std::ostream& operator<<(std::ostream &os, const std::pair<T, U>& p) { os << "<" << p.first << "," << p.second << ">"; return os; }
template<class T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) { for(auto x : vec) os << x << " "; return os; }

template <typename T> bool mycomp(std::pair<T, T> x, std::pair<T, T> y) { return x.second < y.second; }

int main() {
    lg();
    
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr); // comment out for interactive problems
    std::cout << std::fixed << std::setprecision(12);
    
    int tcs = 1;
    std::cin >> tcs;
    for(int tc = 0; tc < tcs; ++tc) 
    {
        int n;
        std::cin >> n;
        
        std::cout << n << '\n';
    }
    
    return 0;
}