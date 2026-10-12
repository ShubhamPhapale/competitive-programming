#include <bits/stdc++.h>
using namespace std;

#define uid(a, b) uniform_int_distribution<int>(a, b)(rnd)
 
// mt19937 rnd(239);
mt19937 rnd(chrono::steady_clock::now().time_since_epoch().count());

int main(){
    int n = uid(1, 50);
    cout<<n<<endl;
    for(int i=0;i<2*n;i++){
        int x = uid(1, 10);
        cout<<x<<" ";
    }
    cout<<endl;
}