#include "task12/point_locator.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

namespace task12 {
namespace {

long double cross(const Point& a, const Point& b, const Point& c) {
    return (b.x - a.x) * (c.y - a.y) - (b.y - a.y) * (c.x - a.x);
}

bool on_segment(const Point& a, const Point& b, const Point& p, long double eps) {
    if (std::fabsl(cross(a, b, p)) > eps) return false;
    const long double dot = (p.x - a.x) * (p.x - b.x) + (p.y - a.y) * (p.y - b.y);
    return dot <= eps;
}

long double distance_point_segment(const Point& a,
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

struct ContourStats {
    bool boundary = false;
    long double minDistance = std::numeric_limits<long double>::infinity();
    int winding = 0;
    bool parity = false;
};

ContourStats analyze(const Contour& contour, const Point& query, long double eps) {
    ContourStats stats;
    if (contour.vertices.size() < 2) return stats;
    for (size_t i = 0; i < contour.vertices.size(); ++i) {
        const auto& a = contour.vertices[i];
        const auto& b = contour.vertices[(i + 1) % contour.vertices.size()];
        stats.minDistance = std::min(stats.minDistance,
                                     distance_point_segment(a, b, query));
        if (on_segment(a, b, query, eps)) {
            stats.boundary = true;
            stats.minDistance = 0.0L;
            return stats;
        }
        const bool upward = (a.y <= query.y);
        const bool upwardCross = upward && (b.y > query.y);
        const bool downwardCross = (a.y > query.y) && (b.y <= query.y);
        const long double isLeft = cross(a, b, query);
        if (upwardCross && isLeft > eps) ++stats.winding;
        else if (downwardCross && isLeft < -eps) --stats.winding;
        const bool crosses = ((a.y > query.y) != (b.y > query.y));
        if (crosses) {
            const long double t = (query.y - a.y) / (b.y - a.y);
            const long double xEdge = a.x + t * (b.x - a.x);
            if (std::fabsl(xEdge - query.x) <= eps) {
                stats.boundary = true;
                stats.minDistance = 0.0L;
                return stats;
            }
            if (xEdge > query.x) stats.parity = !stats.parity;
        }
    }
    return stats;
}

}  // namespace

long double delta_for_polygon(const Polygon& poly) {
    std::vector<Point> pts;
    for (const auto& contour : poly.contours) {
        for (const auto& v : contour.vertices) pts.push_back(v);
    }
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

Classification classify(const Polygon& poly,
                        const Point& query,
                        long double eps) {
    Classification result;
    long double minDist = std::numeric_limits<long double>::infinity();
    bool boundary = false;
    int windingSolid = 0;
    int windingHole = 0;
    bool paritySolid = false;
    bool parityHole = false;
    for (const auto& contour : poly.contours) {
        if (contour.vertices.size() < 2) continue;
        const auto stats = analyze(contour, query, eps);
        minDist = std::min(minDist, stats.minDistance);
        if (stats.boundary) {
            boundary = true;
            break;
        }
        if (contour.hole) {
            windingHole += stats.winding;
            parityHole ^= stats.parity;
        } else {
            windingSolid += stats.winding;
            paritySolid ^= stats.parity;
        }
    }
    if (boundary) {
        result.region = Region::Boundary;
        result.distance = 0.0L;
        return result;
    }
    const bool insideSolids = paritySolid;
    const bool insideHoles = parityHole;
    const bool inside = insideSolids && !insideHoles;
    result.delta = delta_for_polygon(poly);
    result.distance = std::isfinite(minDist) ? minDist : 0.0L;
    if (inside) {
        result.region = (result.delta > 0.0L && result.distance <= result.delta)
                            ? Region::NearBoundary
                            : Region::Inside;
    } else {
        result.region = (result.delta > 0.0L && result.distance <= result.delta)
                            ? Region::NearBoundary
                            : Region::Outside;
    }
    return result;
}

}  // namespace task12

