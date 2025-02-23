// ----- Dinic’s Maximum Flow -----
struct Dinic {
    struct Edge { int to, rev, cap; };
    int n;
    vector<vector<Edge>> adj;
    vector<int> level, ptr;
    
    Dinic(int n): n(n), adj(n), level(n), ptr(n) {}
    
    void addEdge(int u, int v, int cap) {
        adj[u].push_back({v, (int)adj[v].size(), cap});
        adj[v].push_back({u, (int)adj[u].size()-1, 0});
    }
    
    bool bfs(int s, int t) {
        fill(level.begin(), level.end(), -1);
        level[s] = 0;
        queue<int> q; q.push(s);
        while(!q.empty()){
            int u = q.front(); q.pop();
            for(auto &e : adj[u]){
                if(level[e.to] < 0 && e.cap){
                    level[e.to] = level[u] + 1;
                    q.push(e.to);
                }
            }
        }
        return level[t] != -1;
    }
    
    int dfs(int u, int t, int flow) {
        if (!flow) return 0;
        if (u == t) return flow;
        for(int &i = ptr[u]; i < (int)adj[u].size(); i++){
            Edge &e = adj[u][i];
            if(level[e.to] != level[u] + 1 || e.cap == 0) continue;
            int pushed = dfs(e.to, t, min(flow, e.cap));
            if(pushed){
                e.cap -= pushed;
                adj[e.to][e.rev].cap += pushed;
                return pushed;
            }
        }
        return 0;
    }
    
    int maxFlow(int s, int t) {
        int flow = 0;
        while(bfs(s, t)){
            fill(ptr.begin(), ptr.end(), 0);
            while(int pushed = dfs(s, t, INT_MAX))
                flow += pushed;
        }
        return flow;
    }
};