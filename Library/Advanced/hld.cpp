// ----- Heavy-Light Decomposition (HLD) Skeleton -----
// (Assumes a tree with 0-indexed nodes; further functions such as query/update are built atop a segment tree.)
struct HLD {
    int n;
    vector<vector<int>> adj;
    vector<int> parent, depth, heavy, head, pos;
    int curPos;
    
    HLD(int n): n(n), adj(n), parent(n, -1), depth(n, 0), heavy(n, -1), head(n), pos(n), curPos(0) {}
    
    int dfs(int v) {
        int size = 1, maxSubtree = 0;
        for (int u : adj[v]) {
            if(u == parent[v]) continue;
            parent[u] = v;
            depth[u] = depth[v] + 1;
            int subtreeSize = dfs(u);
            size += subtreeSize;
            if(subtreeSize > maxSubtree){
                maxSubtree = subtreeSize;
                heavy[v] = u;
            }
        }
        return size;
    }
    
    void decompose(int v, int h) {
        head[v] = h;
        pos[v] = curPos++;
        if(heavy[v] != -1)
            decompose(heavy[v], h);
        for (int u : adj[v]) {
            if(u == parent[v] || u == heavy[v])
                continue;
            decompose(u, u);
        }
    }
    
    void init(int root = 0) {
        dfs(root);
        decompose(root, root);
    }
};