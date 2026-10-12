#include <bits/stdc++.h>

// ----- Z-Algorithm -----
std::vector<int> computeZ(const std::string &s) {
    int n = int(s.size());
    std::vector<int> Z(n);
    int L = 0, R = 0;
    for (int i = 1; i < n; i++) {
        if (i <= R)
            Z[i] = std::min(R - i + 1, Z[i - L]);
        while (i + Z[i] < n && s[Z[i]] == s[i + Z[i]])
            Z[i]++;
        if (i + Z[i] - 1 > R) {
            L = i; R = i + Z[i] - 1;
        }
    }
    return Z;
}