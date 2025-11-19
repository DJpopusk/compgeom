#include "task4/convex_hull.hpp"

#include <algorithm>

namespace task4 {
namespace {
inline long double cross(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}
}

bool convex_hull_indices(const std::vector<Point>& pts, HullIndices* hull) {
    hull->clear();
    const int n = static_cast<int>(pts.size());
    if (n == 0) return false;

    std::vector<int> idx(n);
    for (int i=0;i<n;++i) idx[i] = i;

    std::sort(idx.begin(), idx.end(), [&](int i, int j){
        if (pts[i].x < pts[j].x) return true;
        if (pts[i].x > pts[j].x) return false;
        return pts[i].y < pts[j].y;
    });

    std::vector<int> uniq;
    uniq.reserve(n);
    uniq.push_back(idx[0]);
    for (int k=1;k<n;++k) {
        int i = idx[k];
        int j = uniq.back();
        if (pts[i].x == pts[j].x && pts[i].y == pts[j].y) continue;
        uniq.push_back(i);
    }

    if (static_cast<int>(uniq.size()) == 1) {
        hull->push_back(uniq[0]);
        return true;
    }

    std::vector<int> st;
    st.reserve(uniq.size()*2);

    auto crossIdx = [&](int i, int j, int k){
        return cross(pts[i], pts[j], pts[k]);
    };

    for (int id : uniq) {
        while (st.size() >= 2) {
            int k2 = st.back(); st.pop_back();
            int k1 = st.back();
            if (crossIdx(k1, k2, id) > 0.0L) { st.push_back(k2); break; }
        }
        st.push_back(id);
    }

    const size_t lower_size = st.size();
    for (int t = static_cast<int>(uniq.size()) - 2; t >= 0; --t) {
        int id = uniq[t];
        while (st.size() > lower_size) {
            int k2 = st.back(); st.pop_back();
            int k1 = st.back();
            if (crossIdx(k1, k2, id) > 0.0L) { st.push_back(k2); break; }
        }
        st.push_back(id);
    }

    if (!st.empty()) st.pop_back();

    if (st.empty()) {
        hull->push_back(uniq.front());
        if (uniq.back() != uniq.front()) hull->push_back(uniq.back());
    } else {
        *hull = std::move(st);
    }
    return !hull->empty();
}

} 
