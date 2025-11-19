#include "task11/point_locator.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace task11 {
namespace {

long double cross(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool on_segment(const Point& a, const Point& b, const Point& p, long double eps) {
    if (std::fabsl(cross(a, b, p)) > eps) return false;
    const long double dot = (p.x - a.x) * (p.x - b.x) + (p.y - a.y) * (p.y - b.y);
    return dot <= eps;
}

}  // namespace

std::vector<Point> convex_hull(const std::vector<Point>& pts) {
    if (pts.size() < 2) return pts;
    std::vector<Point> sorted = pts;
    std::sort(sorted.begin(), sorted.end(), [](const Point& a, const Point& b) {
        if (a.x < b.x) return true;
        if (a.x > b.x) return false;
        return a.y < b.y;
    });
    sorted.erase(std::unique(sorted.begin(), sorted.end(),
                             [](const Point& a, const Point& b) {
                                 return a.x == b.x && a.y == b.y;
                             }),
                 sorted.end());
    if (sorted.size() <= 2) return sorted;
    std::vector<Point> hull;
    hull.reserve(sorted.size() * 2);
    for (const auto& p : sorted) {
        while (hull.size() >= 2 &&
               cross(hull[hull.size() - 2], hull.back(), p) <= 0.0L) {
            hull.pop_back();
        }
        hull.push_back(p);
    }
    size_t lower = hull.size();
    for (int i = static_cast<int>(sorted.size()) - 2; i >= 0; --i) {
        const auto& p = sorted[i];
        while (hull.size() > lower &&
               cross(hull[hull.size() - 2], hull.back(), p) <= 0.0L) {
            hull.pop_back();
        }
        hull.push_back(p);
    }
    if (!hull.empty()) hull.pop_back();
    return hull;
}

long double point_segment_distance(const Point& a,
                                   const Point& b,
                                   const Point& p) {
    const long double dx = b.x - a.x;
    const long double dy = b.y - a.y;
    const long double len2 = dx * dx + dy * dy;
    if (len2 == 0.0L) {
        const long double tx = p.x - a.x;
        const long double ty = p.y - a.y;
        return std::sqrt(tx * tx + ty * ty);
    }
    const long double t = std::clamp(((p.x - a.x) * dx + (p.y - a.y) * dy) / len2,
                                     0.0L, 1.0L);
    Point proj{a.x + t * dx, a.y + t * dy};
    const long double tx = p.x - proj.x;
    const long double ty = p.y - proj.y;
    return std::sqrt(tx * tx + ty * ty);
}

long double delta_for_points(const std::vector<Point>& pts) {
    if (pts.size() < 2) return 0.0L;
    long double best = std::numeric_limits<long double>::infinity();
    for (size_t i = 0; i < pts.size(); ++i) {
        for (size_t j = i + 1; j < pts.size(); ++j) {
            const long double dx = pts[i].x - pts[j].x;
            const long double dy = pts[i].y - pts[j].y;
            const long double d = std::sqrt(dx * dx + dy * dy);
            best = std::min(best, d);
        }
    }
    if (!std::isfinite(best) || best == 0.0L) return 0.0L;
    return best * 0.1L;
}

Classification classify(const std::vector<Point>& hull,
                        const Point& query,
                        long double eps) {
    Classification result;
    if (hull.size() < 3) {
        result.region = Region::Outside;
        return result;
    }
    result.delta = delta_for_points(hull);
    long double minDist = std::numeric_limits<long double>::infinity();
    bool inside = true;
    for (size_t i = 0; i < hull.size(); ++i) {
        const auto& a = hull[i];
        const auto& b = hull[(i + 1) % hull.size()];
        if (on_segment(a, b, query, eps)) {
            result.region = Region::Boundary;
            result.distance = 0.0L;
            return result;
        }
        const long double cr = cross(a, b, query);
        if (cr < -eps) inside = false;
        minDist = std::min(minDist, point_segment_distance(a, b, query));
    }
    result.distance = minDist;
    if (inside) {
        result.region = (result.delta > 0.0L && minDist <= result.delta)
                            ? Region::NearBoundary
                            : Region::Inside;
    } else {
        result.region = (result.delta > 0.0L && minDist <= result.delta)
                            ? Region::NearBoundary
                            : Region::Outside;
    }
    return result;
}

}  // namespace task11

