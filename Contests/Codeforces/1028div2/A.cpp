// Problem A
#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define ld long double

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int t = 1;
    cin >> t;
    
    while (t--) {
        int a, b, c, d;
        cin >> a >> b >> c >> d;
        
        a = min(a, c);
        b = min(b, d);
        
        if(a < b) cout << "Flower" << endl;
        else cout << "Gellyfish" << endl;
    }
    
    return 0;
}