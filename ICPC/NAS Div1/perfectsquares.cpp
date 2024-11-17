/**
 *    author:  shubhamphpefy
 *    created: 17.11.2024 04:02:33
**/

// solution for https://open.kattis.com/contests/nasouth24d1open/problems/perfectsquares

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
const int INF=1e9;
const double EPS = 1e-9;
const double PI=3.14159265358979323846264338327950288419716939937510582097494459230;

template<class T, class U> istream& operator>>(istream &is, pair<T, U>& p) { is >> p.first >> p.second; return is; }
template<class T> istream& operator>>(istream& is, vector<T>& vec) { for(auto &x : vec) cin >> x; return is; } 
template<class T, class U> ostream& operator<<(ostream &os, const pair<T, U>& p) { os << "<" << p.first << "," << p.second << ">"; return os; }
template<class T> ostream& operator<<(ostream& os, const vector<T>& vec) { for(auto x : vec) os << x << " "; return os; }

template <typename T> bool mycomp(pair<T, T> x, pair<T, T> y) { return x.second < y.second; }

template <typename T>
T pow2 (T base, T exp) { T result = 1; while (exp > 0) { result = (exp % 2) ? result * base : result; base *= base; exp /= 2; } return result; }

using ll = long long;
using ull = unsigned long long;

// Function Prototypes
ull modmul(ull a, ull b, ull m);
ull modpow(ull b, ull e, ull mod);
bool isprime(ull n);
ull pollard(ull n);
vector<ull> factor(ull n);
bool isok(ll n);
pair<ll, ll> check(ll n);

// Modular multiplication
ull modmul(ull a, ull b, ull m) {
    ll ret = a * b - m * (ull)(1.l / m * a * b);
    return ret + m * (ret < 0) - m * (ret >= (ll)m);
}

// Modular exponentiation
ull modpow(ull b, ull e, ull mod) {
    ull ans = 1;
    for (; e; b = modmul(b, b, mod), e /= 2) {
        if (e & 1) ans = modmul(ans, b, mod);
    }
    return ans;
}

// Miller-Rabin Primality Test
bool isprime(ull n) {
    if (n < 2 || n % 6 % 4 != 1) return (n | 1) == 3;
    ull A[] = {2, 325, 9375, 28178, 450775, 9780504, 1795265022};
    ull s = __builtin_ctzll(n - 1), d = n >> s;
    for (ull a : A) {
        ull p = modpow(a % n, d, n), i = s;
        if (p <= 1 || p == n - 1) continue;
        while (p != 1 && p != n - 1 && a % n && i--) {
            p = modmul(p, p, n);
        }
        if (p != n - 1 && i != s) return false;
    }
    return true;
}

// Pollard's Rho for integer factorization
ull pollard(ull n) {
    ull x = 0, y = 0, t = 30, prd = 2, i = 1, q;
    auto f = [&](ull a) { return modmul(a, a, n) + i; };
    while (t++ % 40 || gcd(prd, n) == 1) {
        if (x == y) x = ++i, y = f(x);
        if ((q = modmul(prd, max(x, y) - min(x, y), n))) prd = q;
        x = f(x), y = f(f(y));
    }
    return gcd(prd, n);
}

// Factorizes a number into prime factors
vector<ull> factor(ull n) {
    if (n == 1) return {};
    if (isprime(n)) return {n};
    ull x = pollard(n);
    auto l = factor(x);
    auto r = factor(n / x);
    l.insert(l.end(), all(r));
    return l;
}

// Checks if a number can be expressed as the sum of two squares
pair<ll, ll> check(ll n) {
    ll j = sqrt(n) + 10;
    for (ll i = 0; i * i * 2 <= n; i++) {
        while (i * i + j * j > n && j >= 1) {
            j--;
        }
        if (i * i + j * j == n) {
            return {i, j};
        }
    }
    return {-1, -1};
}

// Validates if a number satisfies the condition for being expressed as a sum of squares
bool isok(ll n) {
    vector<ull> facs = factor(n);
    map<ull, int> cnt;
    for (auto a : facs) cnt[a]++;
    for (auto a : cnt) {
        if (a.first % 4 == 3 && a.second % 2 == 1) return false;
    }
    return true;
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(nullptr);
    cout << fixed << setprecision(12);

    ll n;
    cin >> n;

    ll cap = sqrt(n) + 10;
    ll lo = sqrt(n / 3) - 10;

    lo = max(0ll, lo);
    ll ni = n;

    while (ni % 4 == 0) ni /= 4;
    if (ni % 8 == 7) {
        d1(-1);
        return 0;
    }

    for (ll curr = lo; curr <= cap; curr++) {
        ll m = n - curr * curr;
        if (m < 0) continue;
        if (!isok(m)) continue;
        pair<ll, ll> v = check(m);
        if (v.first != -1) {
            d3(v.first, v.second, curr);
            return 0;
        }
    }

    d1(-1);
    return 0;
}
