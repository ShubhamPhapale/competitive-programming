#ifndef DEBUG_H
#define DEBUG_H

#include <iostream>
#include <vector>
#include <string>
#include <tuple>
#include <stack>
#include <queue>
#include <algorithm>

inline void __print(int x) { std::cerr << x; }
inline void __print(long x) { std::cerr << x; }
inline void __print(long long x) { std::cerr << x; }
inline void __print(unsigned x) { std::cerr << x; }
inline void __print(unsigned long x) { std::cerr << x; }
inline void __print(unsigned long long x) { std::cerr << x; }
inline void __print(short x) { std::cerr << x; }
inline void __print(unsigned short x) { std::cerr << x; }
inline void __print(float x) { std::cerr << x; }
inline void __print(double x) { std::cerr << x; }
inline void __print(long double x) { std::cerr << x; }
inline void __print(char x) { std::cerr << '\'' << x << '\''; }
inline void __print(const char *x) { std::cerr << '\"' << x << '\"'; }
inline void __print(const std::string &x) { std::cerr << '\"' << x << '\"'; }
inline void __print(bool x) { std::cerr << (x ? "true" : "false"); }

inline void __print(__int128 x) {
    if (x < 0) { std::cerr << '-'; x = -x; }
    if (x > 9) __print(x / 10);
    std::cerr << (char)('0' + (x % 10));
}
inline void __print(unsigned __int128 x) {
    if (x > 9) __print(x / 10);
    std::cerr << (char)('0' + (x % 10));
}

template <typename T, typename V>
void __print(const std::pair<T, V> &x);

template <typename... Args>
void __print(const std::tuple<Args...> &t);

template <typename T>
void __print(std::stack<T> s);

template <typename T>
void __print(std::queue<T> q);

template <typename T, typename Container, typename Compare>
void __print(std::priority_queue<T, Container, Compare> pq);

template <typename T>
void __print(const T &x);

template <typename T, typename V>
void __print(const std::pair<T, V> &x) {
    std::cerr << '{'; __print(x.first); std::cerr << ','; __print(x.second); std::cerr << '}';
}

template <typename... Args>
void __print(const std::tuple<Args...> &t) {
    int f = 0;
    std::cerr << '{';
    std::apply([&](const auto &...args) {
        ((std::cerr << (f++ ? "," : ""), __print(args)), ...);
    }, t);
    std::cerr << '}';
}

template <typename T>
void __print(const T &x) {
    int f = 0; std::cerr << '{';
    for (const auto &i : x) std::cerr << (f++ ? "," : ""), __print(i);
    std::cerr << '}';
}

template <typename T>
void __print(std::stack<T> s) {
    std::vector<T> v;
    while (!s.empty()) { v.push_back(s.top()); s.pop(); }
    std::reverse(v.begin(), v.end());
    __print(v);
}

template <typename T>
void __print(std::queue<T> q) {
    std::vector<T> v;
    while (!q.empty()) { v.push_back(q.front()); q.pop(); }
    __print(v);
}

template <typename T, typename Container, typename Compare>
void __print(std::priority_queue<T, Container, Compare> pq) {
    std::vector<T> v;
    while (!pq.empty()) { v.push_back(pq.top()); pq.pop(); }
    __print(v);
}

inline void _print() { std::cerr << "]\n"; }

template <typename T, typename... V>
void _print(T t, V... v) {
    __print(t);
    if (sizeof...(v)) std::cerr << ", ";
    _print(v...);
}

#define dbg(x...) std::cerr << "[" << #x << "] = ["; _print(x)

#endif // DEBUG_H
