#include <bits/stdc++.h>
#include "../algo/debug.h"

using namespace std;

int main() {
    int l = 1, r = 1000000;
    while (l != r) {
        int mid = (l + r + 1) / 2;
        cout<<mid<<endl;

        char response[3];
        scanf("%s", response);
        if (strcmp(response, "<") == 0)
            r = mid - 1;
        else
            l = mid;
        
        dbg(l, mid);
    }
    
    cout<<"! "<<l<<endl;
}