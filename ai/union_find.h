// -*- C++ -*-
#ifndef UNION_FIND_H_
#define UNION_FIND_H_

#include <vector>

#include "../core/json.h"

class UnionFind {
public:
    std::vector<int> data;
    UnionFind(int size);
    UnionFind(std::vector<int> v);
    UnionFind(Json v);
    bool unionSet(int x, int y);
    bool findSet(int x, int y);
    int root(int x);
    int size(int x);
    Json serialize() const;
    static UnionFind deserialize(const Json& json);
};

#endif  // UNION_FIND_H_
