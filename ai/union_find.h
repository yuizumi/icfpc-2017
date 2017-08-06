// -*- C++ -*-
#ifndef UNION_FIND_H_
#define UNION_FIND_H_

#include <vector>

#include "../core/json.h"

class UnionFind {
public:
    explicit UnionFind(int size);
    UnionFind() = default;  // For Json::get<>().
   
    bool UnionSet(int x, int y);
    bool FindSet(int x, int y);
    int root(int x);
    int size(int x);

private:
    friend void from_json(const Json& json, UnionFind& value);
    friend void to_json(Json& json, const UnionFind& value);

    std::vector<int> data_;
};

void from_json(const Json& json, UnionFind& value);
void to_json(Json& json, const UnionFind& value);

#endif  // UNION_FIND_H_
