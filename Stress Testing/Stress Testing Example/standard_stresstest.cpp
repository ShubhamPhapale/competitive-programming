/**
 *    author:  shubhamphpefy
 *    created: 31.08.2024 18:44:59
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
#include "algo/debug.h"
#include "algo/log.h"
#else
#define dbg(...) 42
#define lg(...) 42
#endif

// mt19937 rnd(239);
mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

#define ll long long
// #define ld long double
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

// Brute force solution for verification
ll bruteForce(const vector<ll>& v) {
    int n = v.size();
    ll count = 0;
    for(int i = 0; i < n; i++) {
        for(int j = i; j < n; j++) {
            if(j - i <= 1) {
                count++;
                continue;
            }
            bool isAP = true;
            ll diff = v[i+1] - v[i];
            for(int k = i+2; k <= j; k++) {
                if(v[k] - v[k-1] != diff) {
                    isAP = false;
                    break;
                }
            }
            if(isAP) count++;
        }
    }
    return count;
}

// Your original solution
ll solution(const vector<ll>& v) {
    int n = v.size();
    
    if(n==1)return 1;
    
    ll cnt = n;
        
    vector<ll> dif(n-1);
    for(int i = 0; i < n-1; i++) {
        dif[i] = v[i+1] - v[i];
    }
    
    ll len = 1;
    for(int i = 1; i < n-1; i++) {
        if(dif[i] == dif[i-1]) {
            len++;
        } else {
            cnt += (len * (len + 1)) / 2;
            len = 1;
        }
    }
    cnt += (len * (len + 1)) / 2;
    
    return cnt;
}

// Stress test function
void stressTest() {
    mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
    int testCases = 1000;  // Number of test cases to generate
    int maxN = 100;  // Maximum array size
    int maxVal = 1000000000;  // Maximum value in the array

    for(int t = 1; t <= testCases; t++) {
        int n = rng() % maxN + 1;  // Random size between 1 and maxN
        vector<ll> v(n);
        for(int i = 0; i < n; i++) {
            v[i] = rng() % maxVal + 1;  // Random values between 1 and maxVal
        }

        ll result1 = solution(v);
        ll result2 = bruteForce(v);

        if(result1 != result2) {
            cout << "Mismatch found in test case " << t << ":\n";
            cout << "Array size: " << n << "\n";
            cout << "Array: ";
            for(int i = 0; i < n; i++) {
                cout << v[i] << " ";
            }
            cout << "\n";
            cout << "Your solution: " << result1 << "\n";
            cout << "Correct answer: " << result2 << "\n";
            return;
        }

        if(t % 100 == 0) {
            cout << "Passed " << t << " test cases\n";
        }
    }

    cout << "All test cases passed!\n";
}

int main() {
    stressTest();
    return 0;
}