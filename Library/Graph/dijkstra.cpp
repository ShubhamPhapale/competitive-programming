// ----- Dijkstra’s Algorithm (Single-Source Shortest Paths) -----
vector<long long> dijkstra(int n, int source, const vector<vector<pair<int,int>>>& adj) {
    const long long INF = 1e18;
    vector<long long> dist(n, INF);
    dist[source] = 0;
    // (distance, node)
    priority_queue<pair<long long,int>, vector<pair<long long,int>>, greater<pair<long long,int>>> pq;
    pq.push({0, source});
    while(!pq.empty()){
        auto [d, u] = pq.top(); pq.pop();
        if(d != dist[u]) continue;
        for(auto &edge : adj[u]){
            int v = edge.first, w = edge.second;
            if(dist[u] + w < dist[v]){
                dist[v] = dist[u] + w;
                pq.push({dist[v], v});
            }
        }
    }
    return dist;
}