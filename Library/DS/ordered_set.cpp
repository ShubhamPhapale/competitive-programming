#include <bits/stdc++.h>
#include <ext/pb_ds/assoc_container.hpp>
#include <ext/pb_ds/tree_policy.hpp>

using namespace __gnu_pbds;

template <typename T>
using ordered_set = tree<T, null_type, std::less<T>, rb_tree_tag, tree_order_statistics_node_update>;

template <typename T>
using ordered_multiset = tree<T, null_type, std::less_equal<T>, rb_tree_tag, tree_order_statistics_node_update>;

// Usage:
// ordered_set<int> os;
// os.insert(x);
// os.find_by_order(k); // Iterator to k-th smallest element (0-indexed)
// os.order_of_key(x);  // Count of elements strictly smaller than x
