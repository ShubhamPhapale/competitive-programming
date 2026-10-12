// Problem B
#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define ld long double

#ifdef LOCAL
#include "algo/debug.h"
#else
#define dbg(...) 42
#endif

const int MAXN = 100000 + 5;
const int MOD = 998244353;
ll prepow2[MAXN];

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int t = 1;
    cin >> t;
    
    prepow2[0] = 1;
    for(int i = 1; i < MAXN; i++){
        prepow2[i] = (prepow2[i-1] * 2) % MOD;
    }
    
    while (t--) {
        int n;
        cin >> n;

        vector<int> p(n), q(n), bp(n), bq(n);
        for(int i = 0; i < n; i++) cin >> p[i];
        for(int i = 0; i < n; i++) cin >> q[i];

        int bpi = 0, bqi = 0;
        bp[0] = bq[0] = 0;
        for(int i = 1; i < n; i++){
            if(p[i] > p[bpi]) bpi = i;
            bp[i] = bpi;
            
            if(q[i] > q[bqi]) bqi = i;
            bq[i] = bqi;
        }

        for(int i = 0; i < n; i++){
            int j = bp[i];
            int a1 = p[j], b1 = q[i - j];
            int x1 = max(a1, b1), y1 = min(a1, b1);

            int k = bq[i];
            int a2 = p[i - k], b2 = q[k];
            int x2 = max(a2, b2), y2 = min(a2, b2);

            int x, y;
            if(x1 > x2 || (x1 == x2 && y1 > y2)){
                x = x1; 
                y = y1;
            } else {
                x = x2; 
                y = y2;
            }

            ll val = (prepow2[x] + prepow2[y]) % MOD;
            cout << val << " ";
        }
        cout << endl;

    }
    
    return 0;
}