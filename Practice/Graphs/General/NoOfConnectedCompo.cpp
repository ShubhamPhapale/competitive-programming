#include<bits/stdc++.h> 
using namespace std;

#define ll long long
#define mod 1000000007

void dfs(int s, vector<int> adj[], bool visited[])
{
	if(visited[s]) return;
    visited[s] = true;
    cout<<s<<endl;
    //process node s
    for(auto u: adj[s])
    {
        dfs(u, adj, visited);
    }
}

void solve() 
{
	int n,m;
	cin>>n>>m;
	vector<int> adj[n+1];
	bool visited[n+1];
	memset(visited, false, sizeof(visited));
	for(int i=1;i<=m;i++)
	{
		int x,y;
		cin>>x>>y;
		adj[x].push_back(y);
		adj[y].push_back(x);
	}
	int ans = 0;
	for(int i=1;i<=n;i++)
	{
		if(!visited[i])
		{
			ans++;
			cout<<"The connected component "<<ans<<" includes :"<<endl;
			dfs(i, adj, visited);
		}
	}
	cout<<"Number of connected components is "<<ans<<endl;
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