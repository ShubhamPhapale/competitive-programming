#include<bits/stdc++.h> 
using namespace std;

#define ll long long
#define mod 1000000007

#define INF 1000000000

void BellFordSP(int x, int n, vector<tuple<int, int, int>> edges, int distance[])
{
	for (int i = 1; i <= n; i++) distance[i] = INF;
    distance[x] = 0;
    for (int i = 1; i <= n; i++) 
    {
        for (auto e : edges) {
           int a, b, w;
           tie(a, b, w) = e;
           distance[b] = min(distance[b], distance[a]+w);
        }
    }
}

void solve() 
{
	int n,m;
	cin>>n>>m;
	
	vector<tuple<int, int, int>> edges;
	int distance[n+1];
	
	for(int i=1;i<=m;i++)
	{
		int x,y,w;
		cin>>x>>y>>w;
		edges.push_back(make_tuple(x,y,w));
		edges.push_back(make_tuple(y,x,w));//eliminate for directed graph
	}
	
	int x;
	cin>>x;
	
	BellFordSP(x, n, edges, distance);
	
	for(int i=1;i<=n;i++)
	{
		cout<<distance[i]<<endl;
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