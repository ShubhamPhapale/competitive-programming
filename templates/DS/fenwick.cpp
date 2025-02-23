// ----- Fenwick Tree (Binary Indexed Tree) -----
struct FenwickTree {
    int n;
    vector<long long> fenw;
    FenwickTree(int n): n(n), fenw(n+1, 0) {}
    
    // point update: add delta to index i (0-indexed)
    void update(int i, long long delta) {
        for(++i; i <= n; i += i & -i)
            fenw[i] += delta;
    }
    
    // prefix sum query [0, i]
    long long query(int i) {
        long long sum = 0;
        for(++i; i > 0; i -= i & -i)
            sum += fenw[i];
        return sum;
    }
    
    // range query [l, r]
    long long rangeQuery(int l, int r) {
        return query(r) - (l ? query(l-1) : 0);
    }
};