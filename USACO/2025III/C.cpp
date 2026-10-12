// Problem C
#include <iostream>
#include <vector>
#include <algorithm>
using namespace std;
 
int main(){
    ios::sync_with_stdio(false);
    cin.tie(nullptr);
    
    int T; 
    cin >> T;
    while(T--){
        int N, K;
        cin >> N >> K;
        vector<int> seq(N);
        for (int i = 0; i < N; i++){
            cin >> seq[i];
        }
        
        // dp[l][r] = minimum number of PRINT statements needed for subarray seq[l...r]
        const int INF = 1e9;
        vector<vector<int>> dp(N, vector<int>(N, INF));
        
        for (int i = 0; i < N; i++){
            dp[i][i] = 1;
        }
        
        for (int L = 2; L <= N; L++){
            for (int l = 0; l + L - 1 < N; l++){
                int r = l + L - 1;
                for (int m = l; m < r; m++){
                    dp[l][r] = min(dp[l][r], dp[l][m] + dp[m+1][r]);
                }
                int len = L;
                for (int d = 1; d <= len/2; d++){
                    if(len % d != 0) continue; 
                    int t = len / d;
                    if(t < 2) continue; 
                    bool valid = true;
                    for (int i = 1; i < t && valid; i++){
                        for (int j = 0; j < d; j++){
                            if(seq[l+j] != seq[l + i*d + j]){
                                valid = false;
                                break;
                            }
                        }
                    }
                    if(valid){
                        dp[l][r] = min(dp[l][r], dp[l][l+d-1]);
                    }
                }
            }
        }
        
        cout << (dp[0][N-1] <= K ? "YES" : "NO") << "\n";
    }
    return 0;
}
