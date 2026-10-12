// Problem B
/**
 *    author:  shubhamphpefy
 *    created: 22.02.2025 17:21:26
**/
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
 
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
 
    int N;
    cin >> N;
    vector<int> FjFavCows(N);
    vector<int> freq(N+1, 0);
    for (int i = 0; i < N; i++){
        cin >> FjFavCows[i];
        if(FjFavCows[i] <= N)
            freq[FjFavCows[i]]++;
    }
 
    vector<int> missing(N+1, 0);
    missing[0] = 0;
    for (int i = 1; i <= N; i++){
        missing[i] = missing[i-1] + ( (freq[i-1] == 0) ? 1 : 0 );
    }
 
    // For each target mex m (0 <= m <= N), the minimum number of operations is:
    // answer[m] = max(freq(m), missing[m])
    // (That is, we must remove all occurrences of m (cost = freq(m))
    //  and fill in all missing numbers from 0 to m-1 (cost = missing[m]),
    //  but we can “pair” an operation if an element equal to m is converted into a missing number.)
    for (int m = 0; m <= N; m++){
        int ans = max(freq[m], missing[m]);
        cout << ans << "\n";
    }
 
    return 0;
}
