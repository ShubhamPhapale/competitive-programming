#include<bits/stdc++.h> 
using namespace std;

#define ll long long
#define mod 1000000007

bool dfs(int s, vector<int> adj[], int color[])
{
	if(color[s] == -1) color[s] = 1;
    //cout<<s<<endl;
    //process node s
    for(auto u: adj[s])
    {
    	if(color[u] == -1)
    	{
    		color[u] = 1 - color[s];
    		if(!dfs(u, adj, color))
    		{
    			return false;
    		}
    	}
    	else if(color[u] == color[s])
    	{
    		return false;
    	}
    }
    return true;
}

void solve() 
{
	int n,m;
	cin>>n>>m;
	vector<int> adj[n+1];
	for(int i=1;i<=m;i++)
	{
		int x,y;
		cin>>x>>y;
		adj[x].push_back(y);
		adj[y].push_back(x);
	}
	
	int color[n+1];//0 for blue and 1 for red //No need of visited color will do the job of both coloring and visited
	memset(color, -1, sizeof(color));
	for(int i=1;i<=n;i++)
	{
		if(color[i] == -1)
		{
			if(!dfs(i, adj, color))
			{
				cout<<"Not Bipertite"<<endl;
				return;
			}
		}
	}
	cout<<"Bipertite"<<endl;
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