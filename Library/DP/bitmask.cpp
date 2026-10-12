// ----- Example: Bitmask DP for TSP (n <= 20) -----
const int INF = 1e9;
int tsp(int n, const vector<vector<int>> &dist) {
    int N = 1 << n;
    vector<vector<int>> dp(N, vector<int>(n, INF));
    dp[1][0] = 0; // starting at node 0
    for (int mask = 1; mask < N; mask++){
        for (int u = 0; u < n; u++){
            if(mask & (1 << u)){
                for (int v = 0; v < n; v++){
                    if(!(mask & (1 << v)))
                        dp[mask | (1 << v)][v] = min(dp[mask | (1 << v)][v], dp[mask][u] + dist[u][v]);
                }
            }
        }
    }
    int ans = INF;
    for (int i = 0; i < n; i++){
        ans = min(ans, dp[N-1][i] + dist[i][0]);
    }
    return ans;
}