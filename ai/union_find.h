// -*- C++ -*-
#ifndef UNION_FIND_H_
#define UNION_FIND_H_

#include <vector>

#include "../core/json.h"

class UnionFind {
public:
    explicit UnionFind(int size);
    bool UnionSet(int x, int y);
    bool FindSet(int x, int y);
    int root(int x);
    int size(int x);
    Json Serialize() const;
    static UnionFind Deserialize(const Json& json);

private:
    explicit UnionFind(std::vector<int> data);
    std::vector<int> data_;
};

#endif  // UNION_FIND_H_
