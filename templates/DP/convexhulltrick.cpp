// ----- Convex Hull Trick (for DP optimization; assumes lines added in order) -----
struct Line {
    long long m, b; // y = m*x + b
    double intersectX(const Line &other) const {
        return double(other.b - b) / (m - other.m);
    }
};
 
struct ConvexHullTrick {
    deque<Line> hull;
    void addLine(long long m, long long b) {
        Line newLine = {m, b};
        while(hull.size() >= 2) {
            Line &l1 = hull[hull.size()-2], &l2 = hull.back();
            if(newLine.intersectX(l1) <= l2.intersectX(l1))
                hull.pop_back();
            else break;
        }
        hull.push_back(newLine);
    }
    long long query(long long x) {
        while(hull.size() >= 2 &&
              hull[0].m * x + hull[0].b >= hull[1].m * x + hull[1].b)
            hull.pop_front();
        return hull[0].m * x + hull[0].b;
    }
};