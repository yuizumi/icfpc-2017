#include "union_find.h"

using namespace std;

UnionFind::UnionFind(int size): data(size, -1) {
}

UnionFind::UnionFind(vector<int> v) {
    data = v;
}

bool UnionFind::unionSet(int x, int y) {
    x = root(x);
    y = root(y);
    if(x != y) {
        if(data[y] < data[x])
            swap(x, y);
        data[x] += data[y]; data[y] = x;
    }
    return x != y;
}

bool UnionFind::findSet(int x, int y) {
    return root(x) == root(y);
}

int UnionFind::root(int x) {
    return data[x] < 0 ? x : data[x] = root(data[x]);
}

int UnionFind::size(int x) {
    return -data[root(x)];
}

Json UnionFind::serialize() const {
    return data;
}

UnionFind UnionFind::deserialize(const Json& json) {
    return UnionFind(json.get<vector<int>>());
}
