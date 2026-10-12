#include<bits/stdc++.h> 
using namespace std;

#define ll long long
#define mod 1000000007

const int INF = 1e9;

vector<vector<pair<int, int>>> adj;
vector<int> dist; 
vector<bool> visited;

void dijkstra(int n, int x) {
    dist.resize(n + 1, INF);
    visited.resize(n + 1, false);
    dist[x] = 0;
    
    priority_queue<pair<int, int>> q;
    q.push({0, x});
    
    while (!q.empty()) {
        int a = q.top().second;
        q.pop();
        
        if (visited[a])
            continue;
        
        visited[a] = true;
        
        for (auto u : adj[a]) {
            int b = u.first, w = u.second;
            
            if (dist[a] + w < dist[b]) {
                dist[b] = dist[a] + w;
                q.push({-dist[b], b});
            }
        }
    }
}

void solve() 
{
    int n, m;
    cin >> n >> m;
    
    adj.resize(n + 1);
    
    for (int i = 0; i < m; i++) {
        int u, v, w;
        cin >> u >> v >> w;
        adj[u].push_back({v, w});
        adj[v].push_back({u, w});
    }
    
    int source;
    cin >> source;
    
    dijkstra(n, source);
    
    for (int i = 1; i <= n; i++) {
        if (dist[i] == INF)
            cout << "Node " << i << ": Unreachable\n";
        else
            cout << "Node " << i << ": " << dist[i] << "\n";
    }
}

int main() 
{
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int t = 1;
    // cin >> t;
    
    for (int i = 0; i < t; i++) {
        solve();
    }
    
    return 0;
}
