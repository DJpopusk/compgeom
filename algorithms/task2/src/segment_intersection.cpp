#include "task2/segment_intersection.hpp"

#include <algorithm>

namespace task2 {
namespace {
inline double clampd(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}
}

bool segment_intersection(const Vec2& a, const Vec2& b,
                          const Vec2& c, const Vec2& d,
                          double eps, Vec2* out)
{
    const Vec2 r = b - a;
    const Vec2 s = d - c;
    const double denom = cross(r, s);
    const Vec2 cma = c - a;

    const double tiny = 1e-12;
    if (std::fabs(denom) < tiny) {
        // Параллельны или коллинеарны
        if (std::fabs(cross(cma, r)) > eps * length(r)) return false;

        const double rr = dot(r, r);
        if (rr < tiny) {
            // a==b: вырожденный сегмент — точка
            const double ss = dot(s, s);
            if (ss < tiny) {
                if (length(a - c) <= eps) {
                    if (out) *out = Vec2{(a.x + c.x) * 0.5, (a.y + c.y) * 0.5};
                    return true;
                }
                return false;
            }
            const double t = dot(a - c, s) / ss;
            const double tcl = clampd(t, 0.0, 1.0);
            const Vec2 proj = c + s * tcl;
            if (length(proj - a) <= eps) {
                if (out) *out = proj;
                return true;
            }
            return false;
        }

        const double t0 = dot(c - a, r) / rr;
        const double t1 = dot(d - a, r) / rr;
        const double lo = std::max(0.0, std::min(t0, t1) - eps);
        const double hi = std::min(1.0, std::max(t0, t1) + eps);
        if (lo <= hi) {
            if (out) {
                const double tmin = std::max(0.0, std::min(t0, t1));
                const double tmax = std::min(1.0, std::max(t0, t1));
                const double tmid = clampd(0.5 * (tmin + tmax), 0.0, 1.0);
                *out = a + r * tmid;
            }
            return true;
        }
        return false;
    }

    const double t = cross(cma, s) / denom;
    const double u = cross(cma, r) / denom;

    if (t >= -eps && t <= 1.0 + eps && u >= -eps && u <= 1.0 + eps) {
        if (out) *out = a + r * t;
        return true;
    }
    return false;
}

std::optional<Vec2> segment_intersection(const Vec2& a, const Vec2& b,
                                         const Vec2& c, const Vec2& d,
                                         double eps)
{
    Vec2 out;
    if (segment_intersection(a, b, c, d, eps, &out)) return out;
    return std::nullopt;
}

} // namespace task2
