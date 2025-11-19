#include "task3/point_segment_ld.hpp"

namespace task3 {
namespace {
inline long double length(const Point& v) {
    return std::sqrt(v.x * v.x + v.y * v.y);
}
}

bool point_on_segment(const Point& a, const Point& b,
                      const Point& p, long double eps)
{
    const Point ab{ b.x - a.x, b.y - a.y };
    const long double ab_len = length(ab);
    const long double tiny = 1e-18L;

    if (ab_len < tiny) {
        const long double dx = p.x - a.x;
        const long double dy = p.y - a.y;
        return std::sqrt(dx*dx + dy*dy) <= eps;
    }

    const Point ap{ p.x - a.x, p.y - a.y };
    const long double cross = std::fabsl(ab.x*ap.y - ab.y*ap.x);
    const long double dist = cross / ab_len;
    if (dist > eps) return false;

    const long double dot = ab.x*ap.x + ab.y*ap.y;
    const long double len2 = ab_len * ab_len;
    const long double end_slack = eps * ab_len;

    if (dot < -end_slack)       return false;
    if (dot > len2 + end_slack) return false;

    return true;
}

int point_segment_relation(const Point& a, const Point& b,
                           const Point& p, long double eps)
{
    if (point_on_segment(a, b, p, eps)) return 0;

    const Point ab{ b.x - a.x, b.y - a.y };
    const Point ap{ p.x - a.x, p.y - a.y };
    const long double cross = ab.x*ap.y - ab.y*ap.x;

    if (cross > 0.0L)  return 1;
    if (cross < 0.0L)  return -1;
    return 0;
}

} 
