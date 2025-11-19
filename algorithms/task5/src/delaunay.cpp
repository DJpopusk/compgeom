#include "task5/delaunay.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace task5 {
namespace {
inline long double orient(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

bool in_circumcircle(const Point& A, const Point& B, const Point& C,
                     const Point& P)
{
    const long double ax = A.x - P.x, ay = A.y - P.y;
    const long double bx = B.x - P.x, by = B.y - P.y;
    const long double cx = C.x - P.x, cy = C.y - P.y;

    const long double a2 = ax*ax + ay*ay;
    const long double b2 = bx*bx + by*by;
    const long double c2 = cx*cx + cy*cy;

    const long double det =
        a2 * (bx*cy - by*cx) -
        b2 * (ax*cy - ay*cx) +
        c2 * (ax*by - ay*bx);

    const long double orientABC = orient(A, B, C);
    const long double s = (orientABC > 0.0L) ? 1.0L : (orientABC < 0.0L ? -1.0L : 0.0L);
    const long double eps = 1e-18L;
    return (det * s) > eps;
}
}

bool delaunay_triangulation(const std::vector<Point>& pts,
                            std::vector<Triangle>* triangles)
{
    triangles->clear();
    const int n = static_cast<int>(pts.size());
    if (n < 2) return false;
    if (n == 2) return true;

    long double minx = std::numeric_limits<long double>::infinity();
    long double miny = std::numeric_limits<long double>::infinity();
    long double maxx = -minx;
    long double maxy = -miny;
    for (const auto& p : pts) {
        if (p.x < minx) minx = p.x;
        if (p.x > maxx) maxx = p.x;
        if (p.y < miny) miny = p.y;
        if (p.y > maxy) maxy = p.y;
    }

    const long double dx = maxx - minx;
    const long double dy = maxy - miny;
    const long double delta = (dx > dy ? dx : dy);
    const long double cx = (minx + maxx) * 0.5L;
    const long double cy = (miny + maxy) * 0.5L;
    const long double R  = 4.0L * (delta + 1.0L);

    Point st0{cx - 2.0L*R, cy - R};
    Point st1{cx,           cy + 2.0L*R};
    Point st2{cx + 2.0L*R, cy - R};

    const int i0 = n;
    const int i1 = n + 1;
    const int i2 = n + 2;

    std::vector<Point> ptsTmp = pts;
    ptsTmp.push_back(st0);
    ptsTmp.push_back(st1);
    ptsTmp.push_back(st2);

    std::vector<Triangle> triang;
    triang.emplace_back(Triangle{i0, i1, i2});

    for (int pi = 0; pi < n; ++pi) {
        const Point& P = ptsTmp[pi];

        std::vector<int> bad;
        bad.reserve(triang.size());
        for (int t = 0; t < static_cast<int>(triang.size()); ++t) {
            const Triangle& T = triang[t];
            if (in_circumcircle(ptsTmp[T.a], ptsTmp[T.b], ptsTmp[T.c], P)) {
                bad.push_back(t);
            }
        }

        struct Edge { int u, v; };
        std::vector<Edge> boundary;
        auto add_edge = [&](int u, int v){
            for (size_t k=0;k<boundary.size();++k) {
                if (boundary[k].u == v && boundary[k].v == u) {
                    boundary.erase(boundary.begin()+k);
                    return;
                }
            }
            boundary.push_back({u,v});
        };

        std::vector<char> kill(triang.size(), 0);
        for (int id : bad) {
            const Triangle& T = triang[id];
            add_edge(T.a, T.b);
            add_edge(T.b, T.c);
            add_edge(T.c, T.a);
            kill[id] = 1;
        }

        std::vector<Triangle> kept;
        kept.reserve(triang.size());
        for (int t = 0; t < static_cast<int>(triang.size()); ++t) {
            if (!kill[t]) kept.push_back(triang[t]);
        }
        triang.swap(kept);

        for (const auto& e : boundary) {
            long double o = orient(ptsTmp[e.u], ptsTmp[e.v], P);
            if (o > 0.0L) {
                triang.emplace_back(Triangle{e.u, e.v, pi});
            } else {
                triang.emplace_back(Triangle{e.v, e.u, pi});
            }
        }
    }

    std::vector<Triangle> result;
    result.reserve(triang.size());
    for (const auto& T : triang) {
        if (T.a >= i0 || T.b >= i0 || T.c >= i0) continue;
        result.push_back(T);
    }

    triangles->swap(result);
    return true;
}

} 
