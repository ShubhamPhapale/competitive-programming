#include <bits/stdc++.h>

// ----- Modular Arithmetic & Combinatorics -----
template <int MOD>
struct ModInt {
    long long val;
    ModInt(long long v = 0) { val = v % MOD; if (val < 0) val += MOD; }
    ModInt operator+(const ModInt &o) const { return ModInt(val + o.val); }
    ModInt operator-(const ModInt &o) const { return ModInt(val - o.val); }
    ModInt operator*(const ModInt &o) const { return ModInt(val * o.val % MOD); }
    ModInt pow(long long e) const { ModInt res(1), b(*this); while (e) { if (e & 1) res = res * b; b = b * b; e >>= 1; } return res; }
    ModInt inv() const { return pow(MOD - 2); }
    ModInt operator/(const ModInt &o) const { return *this * o.inv(); }
    ModInt& operator+=(const ModInt &o) { return *this = *this + o; }
    ModInt& operator-=(const ModInt &o) { return *this = *this - o; }
    ModInt& operator*=(const ModInt &o) { return *this = *this * o; }
    friend std::ostream& operator<<(std::ostream &os, const ModInt &m) { return os << m.val; }
    friend std::istream& operator>>(std::istream &is, ModInt &m) { long long v; is >> v; m = ModInt(v); return is; }
};

const int MOD = 1e9 + 7;
using mint = ModInt<MOD>;

std::vector<mint> fact, inv_fact;
void precomp_fact(int n) {
    fact.resize(n + 1);
    inv_fact.resize(n + 1);
    fact[0] = 1;
    for (int i = 1; i <= n; i++) fact[i] = fact[i - 1] * i;
    inv_fact[n] = fact[n].inv();
    for (int i = n; i > 0; i--) inv_fact[i - 1] = inv_fact[i] * i;
}

mint nCr(int n, int r) {
    if (r < 0 || r > n) return mint(0);
    return fact[n] * inv_fact[r] * inv_fact[n - r];
}