#include "task1/point_segment.hpp"
#include <algorithm>

namespace task1 {

bool point_on_segment(const Vec2& a, const Vec2& b,
                      const Vec2& p, double eps)
{
    const Vec2 ab = b - a;
    const double ab_len = length(ab);
    const double tiny = 1e-12;

    if (ab_len < tiny) {
        const Vec2 ap = p - a;
        return length(ap) <= eps;
    }

    const Vec2 ap = p - a;
    const double cross_val = std::fabs(cross(ab, ap));
    const double dist = cross_val / ab_len;
    if (dist > eps) return false;

    const double dot_val = dot(ab, ap);
    const double len2 = ab_len * ab_len;
    const double end_slack = eps * ab_len;

    if (dot_val < -end_slack)       return false;
    if (dot_val > len2 + end_slack) return false;
    return true;
}

int point_segment_relation(const Vec2& a, const Vec2& b,
                           const Vec2& p, double eps)
{
    if (point_on_segment(a, b, p, eps)) return 0;

    const Vec2 ab = b - a;
    const Vec2 ap = p - a;
    const double cross_val = cross(ab, ap);
    if (cross_val > 0)  return 1;
    if (cross_val < 0)  return -1;
    return 0;
}

} // namespace task1
