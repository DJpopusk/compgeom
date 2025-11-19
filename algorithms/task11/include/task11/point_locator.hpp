#pragma once

#include <vector>

namespace task11 {

struct Point {
    long double x = 0.0L;
    long double y = 0.0L;
};

enum class Region { Outside, Inside, Boundary, NearBoundary };

struct Classification {
    Region region = Region::Outside;
    long double distance = 0.0L;
    long double delta = 0.0L;
};

std::vector<Point> convex_hull(const std::vector<Point>& pts);

Classification classify(const std::vector<Point>& hull,
                        const Point& query,
                        long double eps = 1e-12L);

long double delta_for_points(const std::vector<Point>& pts);

long double point_segment_distance(const Point& a,
                                   const Point& b,
                                   const Point& p);

}  // namespace task11

