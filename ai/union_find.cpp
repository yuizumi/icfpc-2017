#include "union_find.h"

using namespace std;

UnionFind::UnionFind(int size) : data_(size, -1) {
}

bool UnionFind::UnionSet(int x, int y) {
    x = root(x);
    y = root(y);
    if (x != y) {
        if (data_[y] < data_[x])
            swap(x, y);
        data_[x] += data_[y]; data_[y] = x;
    }
    return x != y;
}

bool UnionFind::FindSet(int x, int y) {
    return root(x) == root(y);
}

int UnionFind::root(int x) {
    return data_[x] < 0 ? x : data_[x] = root(data_[x]);
}

int UnionFind::size(int x) {
    return -data_[root(x)];
}

void from_json(const Json& json, UnionFind& value) {
    value.data_ = json.get<vector<int>>();
}

void to_json(Json& json, const UnionFind& value) {
    json = value.data_;
}
