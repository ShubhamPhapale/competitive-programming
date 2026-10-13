/**
 *    author:  shubhamphpefy
 *    created: 
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

template <class T> int lwb(const std::vector<T> &a, const T &b) { return int(std::lower_bound(a.begin(), a.end(), b) - a.begin()); }
template <class T> int upb(const std::vector<T> &a, const T &b) { return int(std::upper_bound(a.begin(), a.end(), b) - a.begin()); }

int main() {
	lg();
	
	std::ios_base::sync_with_stdio(false);
	std::cin.tie(nullptr); // comment out for interactive problems
	std::cout << std::fixed << std::setprecision(12);
	
	int tcs = 1;
	std::cin >> tcs;
	for(int tc = 0; tc < tcs; ++tc) 
	{
		
	}
	
	return 0;
}
