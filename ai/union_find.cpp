#include "union_find.h"

using namespace std;

UnionFind::UnionFind(int size) : data_(size, -1) {
}

UnionFind::UnionFind(vector<int> v) : data_(std::move(v)) {
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

Json UnionFind::Serialize() const {
    return data_;
}

UnionFind UnionFind::Deserialize(const Json& json) {
    return UnionFind(json.get<vector<int>>());
}
