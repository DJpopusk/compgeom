#include "task789/convex_boolean.hpp"

#include <algorithm>
#include <cmath>
#include <set>

namespace task789 {
namespace {
inline long double cross(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

inline bool ccw(const Polygon& p) {
    if (p.size() < 3) return true;
    long double s = 0.0L;
    for (size_t i=0;i<p.size();++i) {
        const Point& a = p[i];
        const Point& b = p[(i+1)%p.size()];
        s += a.x*b.y - a.y*b.x;
    }
    return s > 0.0L;
}

inline void cleanup_polygon(Polygon& poly) {
    const long double eps2 = 1e-24L;
    if (poly.size() < 2) return;

    Polygon t; t.reserve(poly.size());
    t.push_back(poly[0]);
    for (size_t i=1;i<poly.size();++i) {
        long double dx = poly[i].x - t.back().x;
        long double dy = poly[i].y - t.back().y;
        if (dx*dx + dy*dy > eps2) t.push_back(poly[i]);
    }
    if (t.size() >= 2) {
        long double dx = t.front().x - t.back().x;
        long double dy = t.front().y - t.back().y;
        if (dx*dx + dy*dy <= eps2) t.pop_back();
    }
    if (t.size() < 3) { poly.swap(t); return; }

    Polygon r; r.reserve(t.size());
    for (size_t i=0;i<t.size();++i) {
        const Point& A = t[(i + t.size() - 1) % t.size()];
        const Point& B = t[i];
        const Point& C = t[(i + 1) % t.size()];
        long double cr = cross(A, B, C);
        if (std::fabsl(cr) > 1e-18L) r.push_back(B);
    }
    if (r.size() >= 3) poly.swap(r); else poly.swap(t);
}

inline Point seg_intersect(const Point& S, const Point& E,
                           const Point& A, const Point& B)
{
    const long double eps = 1e-18L;
    const long double X1=S.x, Y1=S.y, X2=E.x, Y2=E.y;
    const long double X3=A.x, Y3=A.y, X4=B.x, Y4=B.y;
    const long double den = (X1-X2)*(Y3-Y4) - (Y1-Y2)*(X3-X4);
    if (std::fabsl(den) < eps) return S;

    const long double nx = ((X1*Y2 - Y1*X2)*(X3-X4) - (X1-X2)*(X3*Y4 - Y3*X4));
    const long double ny = ((X1*Y2 - Y1*X2)*(Y3-Y4) - (Y1-Y2)*(X3*Y4 - Y3*X4));
    return Point{ nx/den, ny/den };
}

inline Polygon suth_hodg_clip(const Polygon& subject, const Polygon& clipPoly,
                              int insideTest)
{
    if (subject.empty() || clipPoly.size()<3) return {};
    Polygon output = subject;
    const long double eps = 1e-18L;

    for (size_t i=0;i<clipPoly.size();++i) {
        const Point& A = clipPoly[i];
        const Point& B = clipPoly[(i+1)%clipPoly.size()];

        Polygon input; input.swap(output);
        output.clear();
        if (input.empty()) break;

        auto isInside = [&](const Point& P)->bool {
            long double v = (B.x - A.x)*(P.y - A.y) - (B.y - A.y)*(P.x - A.x);
            return (insideTest > 0) ? (v >= -eps) : (v <= eps);
        };

        Point S = input.back();
        bool Sin = isInside(S);
        for (const Point& E : input) {
            bool Ein = isInside(E);
            if (Ein) {
                if (!Sin) output.push_back(seg_intersect(S,E,A,B));
                output.push_back(E);
            } else if (Sin) {
                output.push_back(seg_intersect(S,E,A,B));
            }
            S = E; Sin = Ein;
        }
        cleanup_polygon(output);
    }
    cleanup_polygon(output);
    return output;
}

inline Polygon intersection_convex(const Polygon& A, const Polygon& B) {
    return suth_hodg_clip(A, B, +1);
}

int ensure_point_id(const Point& p, std::vector<Point>& pool) {
    const long double eps = 1e-12L;
    for (size_t i=0;i<pool.size();++i) {
        if (std::fabsl(pool[i].x - p.x) <= eps &&
            std::fabsl(pool[i].y - p.y) <= eps) return static_cast<int>(i);
    }
    pool.push_back(p);
    return static_cast<int>(pool.size()) - 1;
}

enum class PointClass { Outside, OnBoundary, Inside };

PointClass classify_point(const Polygon& poly, const Point& p) {
    if (poly.empty()) return PointClass::Outside;
    const long double eps = 1e-12L;
    bool on = false;
    for (size_t i=0;i<poly.size();++i) {
        const Point& a = poly[i];
        const Point& b = poly[(i+1)%poly.size()];
        long double v = cross(a,b,p);
        if (v < -eps) return PointClass::Outside;
        if (std::fabsl(v) <= eps) on = true;
    }
    return on ? PointClass::OnBoundary : PointClass::Inside;
}

bool segment_intersection(const Point& a1, const Point& a2,
                          const Point& b1, const Point& b2,
                          Point& out, long double& ta, long double& tb)
{
    const long double eps = 1e-18L;
    const long double dx1 = a2.x - a1.x;
    const long double dy1 = a2.y - a1.y;
    const long double dx2 = b2.x - b1.x;
    const long double dy2 = b2.y - b1.y;
    const long double denom = dx1*dy2 - dy1*dx2;
    if (std::fabsl(denom) < eps) return false;

    const long double dx = b1.x - a1.x;
    const long double dy = b1.y - a1.y;
    ta = (dx*dy2 - dy*dx2) / denom;
    tb = (dx*dy1 - dy*dx1) / denom;
    if (ta < -eps || ta > 1.0L + eps || tb < -eps || tb > 1.0L + eps) return false;

    ta = std::clamp(ta, 0.0L, 1.0L);
    tb = std::clamp(tb, 0.0L, 1.0L);
    out = Point{a1.x + ta*dx1, a1.y + ta*dy1};
    return true;
}

struct Edge { int start = 0; int end = 0; };

Polygon polygon_from_edge_loop(const std::vector<Edge>& edges,
                               const std::vector<Point>& pts,
                               int startIdx,
                               std::vector<bool>& used,
                               const std::vector<std::vector<int>>& outgoing)
{
    Polygon poly;
    int current = startIdx;
    const int startVertex = edges[current].start;

    while (true) {
        if (used[current]) break;
        used[current] = true;
        const int s = edges[current].start;
        const int e = edges[current].end;
        poly.push_back(pts[s]);
        if (e == startVertex) break;

        int nextEdge = -1;
        for (int candidate : outgoing[e]) {
            if (!used[candidate]) { nextEdge = candidate; break; }
        }
        if (nextEdge == -1) break;
        current = nextEdge;
    }
    cleanup_polygon(poly);
    return poly;
}

std::vector<Polygon> build_polygons_from_edges(const std::vector<Edge>& edges,
                                               const std::vector<Point>& pts)
{
    std::vector<Polygon> result;
    if (edges.empty()) return result;

    std::vector<std::vector<int>> outgoing(pts.size());
    for (int i=0;i<(int)edges.size();++i) {
        outgoing[edges[i].start].push_back(i);
    }
    std::vector<bool> used(edges.size(), false);
    for (int i=0;i<(int)edges.size();++i) {
        if (used[i]) continue;
        Polygon poly = polygon_from_edge_loop(edges, pts, i, used, outgoing);
        if (poly.size() >= 3) result.push_back(std::move(poly));
    }
    return result;
}

struct IntersectionInfo {
    Point p;
    size_t edgeA = 0;
    long double tA = 0.0L;
    size_t edgeB = 0;
    long double tB = 0.0L;
    int pointId = -1;
};

std::vector<int> augment_polygon(const std::vector<int>& baseIds,
                                 const std::vector<std::vector<size_t>>& perEdge,
                                 const std::vector<IntersectionInfo>& inters,
                                 bool useA)
{
    std::vector<int> augmented;
    const size_t n = baseIds.size();
    augmented.reserve(n * 2);
    for (size_t i=0;i<n;++i) {
        augmented.push_back(baseIds[i]);
        auto ids = perEdge[i];
        std::sort(ids.begin(), ids.end(), [&](size_t lhs, size_t rhs){
            long double al = useA ? inters[lhs].tA : inters[lhs].tB;
            long double ar = useA ? inters[rhs].tA : inters[rhs].tB;
            return al < ar;
        });
        for (size_t idx : ids) {
            int id = inters[idx].pointId;
            if (id != augmented.back()) augmented.push_back(id);
        }
    }
    return augmented;
}

std::vector<Polygon> difference_convex(const Polygon& A, const Polygon& B) {
    std::vector<Polygon> result;
    if (A.empty()) return result;

    auto classifyA0 = classify_point(B, A[0]);
    auto classifyB0 = classify_point(A, B[0]);

    std::vector<Point> idPoints;
    idPoints.reserve(A.size() + B.size());
    std::vector<int> baseIdsA, baseIdsB;
    baseIdsA.reserve(A.size());
    baseIdsB.reserve(B.size());
    for (const auto& p : A) baseIdsA.push_back(ensure_point_id(p, idPoints));
    for (const auto& p : B) baseIdsB.push_back(ensure_point_id(p, idPoints));

    std::vector<IntersectionInfo> intersections;
    std::vector<std::vector<size_t>> intsOnA(A.size());
    std::vector<std::vector<size_t>> intsOnB(B.size());
    std::set<std::tuple<int,size_t,size_t>> seen;

    for (size_t i=0;i<A.size();++i) {
        Point a1 = A[i];
        Point a2 = A[(i+1)%A.size()];
        for (size_t j=0;j<B.size();++j) {
            Point b1 = B[j];
            Point b2 = B[(j+1)%B.size()];
            Point inter;
            long double ta = 0.0L, tb = 0.0L;
            if (!segment_intersection(a1,a2,b1,b2, inter, ta, tb)) continue;
            int pid = ensure_point_id(inter, idPoints);
            auto key = std::make_tuple(pid, i, j);
            if (seen.count(key)) continue;
            seen.insert(key);
            IntersectionInfo info;
            info.p = inter;
            info.edgeA = i;
            info.edgeB = j;
            info.tA = ta;
            info.tB = tb;
            info.pointId = pid;
            size_t idx = intersections.size();
            intersections.push_back(info);
            intsOnA[i].push_back(idx);
            intsOnB[j].push_back(idx);
        }
    }

    if (intersections.empty()) {
        if (classifyA0 != PointClass::Outside && classifyB0 != PointClass::Outside) {
            return result;
        }
        if (classifyA0 != PointClass::Outside) {
            return result;
        }
        if (classifyB0 != PointClass::Outside) {
            Polygon hole = B;
            std::reverse(hole.begin(), hole.end());
            result.push_back(A);
            result.push_back(std::move(hole));
            return result;
        }
        result.push_back(A);
        return result;
    }

    auto augmentedA = augment_polygon(baseIdsA, intsOnA, intersections, true);
    auto augmentedB = augment_polygon(baseIdsB, intsOnB, intersections, false);

    auto addEdges = [&](const std::vector<int>& augmented,
                        const Polygon& polyOther,
                        bool reverse,
                        PointClass desired) -> std::vector<Edge>
    {
        std::vector<Edge> edges;
        if (augmented.empty()) return edges;
        for (size_t i=0;i<augmented.size();++i) {
            int s = augmented[i];
            int e = augmented[(i+1)%augmented.size()];
            if (s == e) continue;
            const Point& ps = idPoints[s];
            const Point& pe = idPoints[e];
            Point mid{ (ps.x + pe.x) * 0.5L, (ps.y + pe.y) * 0.5L };
            PointClass cls = classify_point(polyOther, mid);
            bool take = (desired == PointClass::Outside) ? (cls == PointClass::Outside)
                                                        : (cls != PointClass::Outside);
            if (!take) continue;
            if (reverse) edges.push_back(Edge{e,s});
            else edges.push_back(Edge{s,e});
        }
        return edges;
    };

    std::vector<Edge> edges;
    auto aEdges = addEdges(augmentedA, B, false, PointClass::Outside);
    auto bEdges = addEdges(augmentedB, A, true, PointClass::Inside);
    edges.reserve(aEdges.size() + bEdges.size());
    edges.insert(edges.end(), aEdges.begin(), aEdges.end());
    edges.insert(edges.end(), bEdges.begin(), bEdges.end());

    result = build_polygons_from_edges(edges, idPoints);
    return result;
}

inline std::vector<Polygon> union_convex(const Polygon& A, const Polygon& B) {
    std::vector<Polygon> res = difference_convex(A,B);
    std::vector<Polygon> bma = difference_convex(B,A);
    Polygon inter = intersection_convex(A,B);
    res.insert(res.end(),
               std::make_move_iterator(bma.begin()),
               std::make_move_iterator(bma.end()));
    if (!inter.empty()) res.push_back(std::move(inter));
    if (res.empty()) res.push_back(A);
    return res;
}
}

Polygon convex_hull(const std::vector<Point>& pts) {
    if (pts.empty()) return {};

    std::vector<Point> sorted = pts;
    std::sort(sorted.begin(), sorted.end(), [](const Point& a, const Point& b){
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        return a.y < b.y;
    });

    std::vector<Point> uniq;
    uniq.reserve(sorted.size());
    uniq.push_back(sorted[0]);
    for (size_t i=1;i<sorted.size();++i) {
        if (sorted[i].x == uniq.back().x && sorted[i].y == uniq.back().y) continue;
        uniq.push_back(sorted[i]);
    }

    if (uniq.size() == 1) return uniq;

    std::vector<Point> st;
    st.reserve(uniq.size()*2);
    for (const auto& pt : uniq) {
        while (st.size() >= 2) {
            Point p2 = st.back(); st.pop_back();
            Point p1 = st.back();
            if (cross(p1,p2,pt) > 0.0L) { st.push_back(p2); break; }
        }
        st.push_back(pt);
    }

    size_t lower = st.size();
    for (int i = static_cast<int>(uniq.size()) - 2; i >= 0; --i) {
        const auto& pt = uniq[i];
        while (st.size() > lower) {
            Point p2 = st.back(); st.pop_back();
            Point p1 = st.back();
            if (cross(p1,p2,pt) > 0.0L) { st.push_back(p2); break; }
        }
        st.push_back(pt);
    }

    if (!st.empty()) st.pop_back();
    return st;
}

std::vector<Polygon> boolean_operation(const Polygon& hullA,
                                       const Polygon& hullB,
                                       Operation op)
{
    Polygon A = hullA;
    Polygon B = hullB;
    if (A.empty() || B.empty()) return {};

    if (!ccw(A)) std::reverse(A.begin(), A.end());
    if (!ccw(B)) std::reverse(B.begin(), B.end());

    if (op == Operation::Intersection) {
        Polygon inter = intersection_convex(A,B);
        if (inter.empty()) return {};
        return { std::move(inter) };
    }
    if (op == Operation::DifferenceAB) {
        return difference_convex(A,B);
    }

    return union_convex(A,B);
}

} 
