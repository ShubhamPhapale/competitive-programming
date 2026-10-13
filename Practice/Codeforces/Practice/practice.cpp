/**
 *    author:  shubhamphpefy
 *    created: 13.10.2026 22:50:46
**/
#include <bits/stdc++.h>

#ifdef LOCAL
#include "debug.h"
#include "log.h"
#else
#define dbg(...) 42
#define lg(...) 42
#endif

std::mt19937 rnd(std::chrono::steady_clock::now().time_since_epoch().count());
#define uid(a, b) std::uniform_int_distribution<int>(a, b)(rnd)

using ll = long long;
using ld = long double;

constexpr int MOD = 1e9 + 7;
constexpr int MOD2 = 998244353;
constexpr int INF = 1e9;
constexpr ll LLINF = 1e18;
constexpr double EPS = 1e-9;

template<class T, class U> std::istream& operator>>(std::istream &is, std::pair<T, U>& p) { is >> p.first >> p.second; return is; }
template<class T> std::istream& operator>>(std::istream& is, std::vector<T>& vec) { for(auto &x : vec) is >> x; return is; } 
template<class T, class U> std::ostream& operator<<(std::ostream &os, const std::pair<T, U>& p) { os << "<" << p.first << "," << p.second << ">"; return os; }
template<class T> std::ostream& operator<<(std::ostream& os, const std::vector<T>& vec) { for(auto x : vec) os << x << " "; return os; }

int main() {
    lg();
    
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(nullptr); // comment out for interactive problems
    std::cout << std::fixed << std::setprecision(12);
    
    int tcs = 1;
    std::cin >> tcs;
    for(int tc = 0; tc < tcs; ++tc) 
    {
        ll a, b, c, m;
        std::cin >> a >> b >> c >> m;
        
        ll gcdab = std::gcd(a, b), gcdbc = std::gcd(b, c), gcdca = std::gcd(c, a);
        ll gcdabc = std::gcd(gcdab, gcdbc);
        
        ll abc = m / gcdabc, ab = m / gcdab, bc = m / gcdbc, ca = m / gcdca;
        a = m / a, b = m / b, c = m / c;
        
        ll ansa = 0, ansb = 0, ansc = 0;
        
        ll all3 = a + b + c - ab - bc - ca + abc;
        ll a2 = a - ab - ca, b2 = b - ab - bc, c2 = c - bc - ca;
        
        ansa = all3 * 2 + a2 * 3 + a * 6;
        ansb = all3 * 2 + b2 * 3 + b * 6;
        ansc = all3 * 2 + c2 * 3 + c * 6;
        
        std::cout << ansa << " " << ansb << " " << ansc << '\n';
    }
    
    return 0;
}