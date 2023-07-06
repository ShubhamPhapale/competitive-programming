#include<bits/stdc++.h> 

using namespace std;

#define ll long long
#define mod 1000000007

//const int N = 2e5 + 5;

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
    int N;// Number of nodes
    int M;// Number of edges
    cin>>N>>M;
    
    vector<int> adj[N+1];
    bool visited[N+1];
    
    for(int i=1;i<=N;i++)
    {
        visited[i]=false;
    }
    
    for(int i=1;i<=M;i++)
    {
        int x,y;
        cin>>x>>y;
        adj[x].push_back(y);
        adj[y].push_back(x);//eliminate for directed graph
    }
    
    for(int i=1;i<=N;i++)
    {
        dfs(i, adj, visited);
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