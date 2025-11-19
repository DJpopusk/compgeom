#include "task789/convex_boolean.hpp"

#include <algorithm>
#include <cmath>

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
inline Polygon difference_convex(const Polygon& A, const Polygon& B) {
    return suth_hodg_clip(A, B, -1);
}
inline std::vector<Polygon> union_convex(const Polygon& A, const Polygon& B) {
    std::vector<Polygon> res;
    Polygon amb = difference_convex(A,B);
    Polygon bma = difference_convex(B,A);
    Polygon inter = intersection_convex(A,B);
    if (!amb.empty()) res.push_back(std::move(amb));
    if (!bma.empty()) res.push_back(std::move(bma));
    if (!inter.empty()) res.push_back(std::move(inter));
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
        Polygon diff = difference_convex(A,B);
        if (diff.empty()) return {};
        return { std::move(diff) };
    }

    return union_convex(A,B);
}

} // namespace task789
