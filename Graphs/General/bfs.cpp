#include<bits/stdc++.h> 
using namespace std;

#define ll long long
#define mod 1000000007
#define INF 1000000000

void bfs(int x, vector<int> adj[], bool visited[], queue<int> q, int distance[])
{
    if(visited[x]) return;
    visited[x] = true;
    distance[x] = 0;
    q.push(x);
    while (!q.empty()) {
       int s = q.front();
       cout<<s<<endl;
       //cout<<"distance :"<<distance[s]<<endl; distance of nodes from the original node which are connected to the original node where we started the bfs //node for which the distance doesn't get printed are not connected to the original node //i.e. are at infinite distance from the original node
       q.pop();
       // process node s
       for (auto u : adj[s]) 
       {
           if (visited[u]) continue;
           visited[u] = true;
           distance[u] = distance[s]+1;
           q.push(u);
       }
    }
}

void solve() 
{
	int N;// Number of nodes
    int M;// Number of edges
    cin>>N>>M;
    
    vector<int> adj[N+1];
    bool visited[N+1];
    queue<int> q;
    int distance[N+1];
    
    for(int i=1;i<=N;i++)
    {
        visited[i]=false;
    }
    
    for(int i=1;i<=M;i++)
    {
        int x,y;
        cin>>x>>y;
        adj[x].push_back(y);
        adj[y].push_back(x);
    }
    
    for(int i=1;i<=N;i++)
    {
        distance[i]=INF;
    }
    
    bfs(1, adj, visited, q, distance);
    
    // for(int i=1;i<=N;i++)
    // {
    //     cout<<"distance of node "<<i<<" from node 1 is "<<distance[i]<<endl;
    // }
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