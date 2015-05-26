#ifndef OPSTACK_H
#define OPSTACK_H

#include <vector>

template <typename T> class opstack {
public:
    bool empty() { return a.empty(); }
    void push(const T &x) { a.push_back(x); }
    void pop() { a.pop_back(); }
    T &top() { return a.back(); }
private:
    std::vector<T> a;
};

#endif
