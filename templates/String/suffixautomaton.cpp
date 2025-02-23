// ----- Suffix Automaton (for substring queries) -----
struct SuffixAutomaton {
    struct State {
        int len, link;
        map<char, int> next;
    };
    vector<State> st;
    int last;
    
    SuffixAutomaton(const string &s) {
        st.resize(2 * s.size());
        st[0].len = 0;
        st[0].link = -1;
        int sz = 1;
        last = 0;
        for (char c : s)
            sz = sa_extend(c, sz);
        st.resize(sz);
    }
    
    int sa_extend(char c, int sz) {
        int cur = sz++;
        st[cur].len = st[last].len + 1;
        int p = last;
        for (; p != -1 && !st[p].next.count(c); p = st[p].link)
            st[p].next[c] = cur;
        if(p == -1){
            st[cur].link = 0;
        } else {
            int q = st[p].next[c];
            if(st[p].len + 1 == st[q].len)
                st[cur].link = q;
            else {
                int clone = sz++;
                st[clone].len = st[p].len + 1;
                st[clone].next = st[q].next;
                st[clone].link = st[q].link;
                for(; p != -1 && st[p].next[c] == q; p = st[p].link)
                    st[p].next[c] = clone;
                st[q].link = st[cur].link = clone;
            }
        }
        last = cur;
        return sz;
    }
};