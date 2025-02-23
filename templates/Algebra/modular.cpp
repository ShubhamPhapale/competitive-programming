// ----- Fast Modular Exponentiation -----
long long modExp(long long base, long long exp, long long mod) {
    long long result = 1 % mod;
    base %= mod;
    while(exp > 0){
        if(exp & 1)
            result = (result * base) % mod;
        base = (base * base) % mod;
        exp >>= 1;
    }
    return result;
}
 
// ----- Extended Euclidean Algorithm -----
tuple<long long, long long, long long> extendedEuclid(long long a, long long b) {
    if(b == 0) return {a, 1, 0};
    auto [g, x1, y1] = extendedEuclid(b, a % b);
    return {g, y1, x1 - (a / b) * y1};
}
 
// ----- Modular Inverse -----
long long modInverse(long long a, long long mod) {
    auto [g, x, y] = extendedEuclid(a, mod);
    if(g != 1) return -1; // inverse doesn't exist
    return (x % mod + mod) % mod;
}

// ----- Chinese Remainder Theorem (CRT) -----
long long crt(const vector<long long>& remainders, const vector<long long>& mods) {
    long long x = 0, M = 1;
    for(auto mod: mods)
        M *= mod;
    for (int i = 0; i < mods.size(); i++){
        long long m_i = mods[i];
        long long M_i = M / m_i;
        long long inv = modInverse(M_i, m_i);
        x = (x + remainders[i] * inv % M * M_i % M) % M;
    }
    return (x + M) % M;
}