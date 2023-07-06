#include<bits/stdc++.h> 
using namespace std;

#define ll long long
#define mod 1000000007

bool dfs(int s, vector<int> adj[], bool visited[], int parent)
{
    //if(visited[s]) return false;
    visited[s] = true;
    //cout<<s<<endl;
    //process node s
    for(auto u: adj[s])
    {
    	if(!visited[u])
    	{
    		if(dfs(u, adj, visited, s))
    		{
    			cout<<"Cycle Present and Passes through "<<s<<endl;
    			return true;
    		}
    	}
    	else if(u != parent)
		{
			cout<<"Cycle Present and Passes through "<<s<<endl;
			return true;
		}
    }
    return false;
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
	
	for(int i=1;i<=n;i++)
	{
		if(!visited[i] && dfs(i, adj, visited, -1))
		{
			cout<<"Cycle detected"<<endl;
			return;
		}
	}
	cout<<"No Cycle detected"<<endl;
	return;
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