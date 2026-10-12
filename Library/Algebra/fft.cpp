#include <bits/stdc++.h>

// ----- Fast Fourier Transform (FFT) & Polynomial Multiplication -----
typedef std::complex<double> cd;
const double PI = std::acos(-1);

void fft(std::vector<cd> & a, bool invert) {
    int n = int(a.size());
    for (int i = 1, j = 0; i < n; i++) {
        int bit = n >> 1;
        for (; j & bit; bit >>= 1)
            j ^= bit;
        j ^= bit;
        if (i < j)
            std::swap(a[i], a[j]);
    }
    for (int len = 2; len <= n; len <<= 1) {
        double ang = 2 * PI / len * (invert ? -1 : 1);
        cd wlen(std::cos(ang), std::sin(ang));
        for (int i = 0; i < n; i += len) {
            cd w(1);
            for (int j = 0; j < len / 2; j++) {
                cd u = a[i + j], v = a[i + j + len / 2] * w;
                a[i + j] = u + v;
                a[i + j + len / 2] = u - v;
                w *= wlen;
            }
        }
    }
    if (invert) {
        for (auto & x : a)
            x /= n;
    }
}
 
std::vector<int> multiplyPolynomials(const std::vector<int> &a, const std::vector<int> &b) {
    std::vector<cd> fa(a.begin(), a.end()), fb(b.begin(), b.end());
    int n = 1;
    while (n < int(a.size() + b.size()) - 1) n <<= 1;
    fa.resize(n); fb.resize(n);
    fft(fa, false); fft(fb, false);
    for (int i = 0; i < n; i++)
        fa[i] *= fb[i];
    fft(fa, true);
    std::vector<int> result(n);
    for (int i = 0; i < n; i++)
        result[i] = int(std::round(fa[i].real()));
    while (!result.empty() && result.back() == 0)
        result.pop_back();
    return result;
}