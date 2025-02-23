struct Point {
    double x, y;
};
 
double cross(const Point &O, const Point &A, const Point &B) {
    return (A.x - O.x) * (B.y - O.y) - (A.y - O.y) * (B.x - O.x);
}
 
// ----- Convex Hull (Andrew’s Monotone Chain) -----
vector<Point> convexHull(vector<Point> pts) {
    int n = pts.size(), k = 0;
    if(n <= 1) return pts;
    sort(pts.begin(), pts.end(), [](const Point &a, const Point &b) {
        return a.x == b.x ? a.y < b.y : a.x < b.x;
    });
    vector<Point> hull(2 * n);
    // Lower hull
    for(int i = 0; i < n; i++){
        while(k >= 2 && cross(hull[k-2], hull[k-1], pts[i]) <= 0)
            k--;
        hull[k++] = pts[i];
    }
    // Upper hull
    for(int i = n - 2, t = k + 1; i >= 0; i--){
        while(k >= t && cross(hull[k-2], hull[k-1], pts[i]) <= 0)
            k--;
        hull[k++] = pts[i];
    }
    hull.resize(k - 1);
    return hull;
}