#include<bits/stdc++.h> 
using namespace std;

#define ll long long
#define mod 1000000007

int dfs(int s, vector<int> adj[], bool visited[])
{
    if(visited[s]) return 0;
    int cnt=1;
    visited[s] = true;
    cout<<s<<endl;
    //process node s
    for(auto u: adj[s])
    {
        cnt+=dfs(u, adj, visited);
    }
    return cnt;
}

void solve() 
{
	int n,m;
	cin>>n>>m;
	
	vector<int> adj[n+1];
	bool visited[n+1];
	
	for(int i=1;i<=m;i++)
	{
		int x,y;
		cin>>x>>y;
		adj[x].push_back(y);
		adj[y].push_back(x);
	}
	
	int ans = dfs(1, adj , visited);
	
	if(ans==n)
	{
		cout<<"Connected"<<endl;
	}
	else
	{
		cout<<"Not Connected"<<endl;
	}
}

int main() 
{
	ios_base:: sync_with_stdio (0);
	cin.tie(0); cout.tie(0);
	
	int t=1;
	// cin>>t;
	for(int i=0;i<t;i++) 
	{
		solve();
	}
}