// Problem C
#include <bits/stdc++.h>
using namespace std;

#define ll long long
#define ld long double

#ifdef LOCAL
#include "algo/debug.h"
#else
#define dbg(...) 42
#endif

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    int t = 1;
    cin >> t;
    
    while (t--) {
        int n;
        cin >> n;
        
        vector<int> v(n);
        for(int i = 0; i < n; i++) cin >> v[i];
    	
    	int globalgcd = v[0];
    	int maxi = globalgcd;
    	for(int i = 1; i < n; i++) { maxi = max(maxi, v[i]); globalgcd = __gcd(globalgcd, v[i]); } 
    	
    	int samecnt = 0;
    	for(int i = 0; i < n; i++) if(v[i] == globalgcd) samecnt++;
    		
    	if(samecnt > 0) cout << n - samecnt << endl;
    	else
    	{
    		// dp over gcd's
    		// dp[d] => minimum number of elements needed to reach gcd = i
    		vector<int> dp(maxi + 1, 1e9), dp2(maxi + 1, 1e9);
    		for(int i = 0; i < n; i++){
    			for(int d = 1; d <= maxi; d++){
                	dp2[d] = dp[d];
            	}
            	
            	dp2[v[i]] = 1;
            	
            	for(int d = 1; d <= maxi; d++){
	                if(dp[d] < 1e9){
	                    int curr = __gcd(d, v[i]);
	                    dp2[curr] = min(dp2[curr], dp[d] + 1);
	                }
                }
                
                for(int d = 1; d <= maxi; d++){
	                dp[d] = dp2[d];
	            }
    		}
    		
    		int minisubsetsize = dp[globalgcd];
	        int ans = n + minisubsetsize - 2;
	        cout << ans << endl;
    	}
   	}
    
    return 0;
}