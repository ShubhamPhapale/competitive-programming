// ----- Sieve of Eratosthenes -----
vector<int> sieve(int n) {
    vector<bool> isPrime(n+1, true);
    vector<int> primes;
    isPrime[0] = isPrime[1] = false;
    for(int i = 2; i <= n; i++){
        if(isPrime[i]){
            primes.push_back(i);
            for(int j = i * i; j <= n; j += i)
                isPrime[j] = false;
        }
    }
    return primes;
}