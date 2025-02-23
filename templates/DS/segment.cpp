// ----- Segment Tree with Lazy Propagation (Range Updates & Queries) -----
struct SegmentTree {
    int n;
    vector<long long> tree, lazy;
    
    SegmentTree(int n) : n(n) {
        tree.assign(4*n, 0);
        lazy.assign(4*n, 0);
    }
    
    void push(int node, int l, int r) {
        if(lazy[node] != 0){
            tree[node] += lazy[node] * (r-l+1);
            if(l != r){
                lazy[node*2] += lazy[node];
                lazy[node*2+1] += lazy[node];
            }
            lazy[node] = 0;
        }
    }
    
    void update(int node, int l, int r, int ql, int qr, long long val) {
        push(node, l, r);
        if(r < ql || l > qr) return;
        if(ql <= l && r <= qr) {
            lazy[node] += val;
            push(node, l, r);
            return;
        }
        int mid = (l + r) / 2;
        update(node*2, l, mid, ql, qr, val);
        update(node*2+1, mid+1, r, ql, qr, val);
        tree[node] = tree[node*2] + tree[node*2+1];
    }
    
    long long query(int node, int l, int r, int ql, int qr) {
        push(node, l, r);
        if(r < ql || l > qr) return 0;
        if(ql <= l && r <= qr) return tree[node];
        int mid = (l + r) / 2;
        return query(node*2, l, mid, ql, qr) + query(node*2+1, mid+1, r, ql, qr);
    }
};