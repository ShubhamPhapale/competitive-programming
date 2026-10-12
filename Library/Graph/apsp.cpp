// ----- Floyd–Warshall Algorithm (All-Pairs Shortest Paths) -----
vector<vector<long long>> floydWarshall(int n, const vector<vector<long long>>& graph) {
    const long long INF = 1e18;
    auto dist = graph; // copy initial graph (with INF for no edge)
    for (int k = 0; k < n; k++){
        for (int i = 0; i < n; i++){
            for (int j = 0; j < n; j++){
                if(dist[i][k] < INF && dist[k][j] < INF)
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
            }
        }
    }
    return dist;
}