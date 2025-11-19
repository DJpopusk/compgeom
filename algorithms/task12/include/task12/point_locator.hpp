#pragma once

#include <vector>

namespace task12 {

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

struct Contour {
    bool hole = false;
    std::vector<Point> vertices;
};

struct Polygon {
    std::vector<Contour> contours;
};

Classification classify(const Polygon& poly,
                        const Point& query,
                        long double eps = 1e-12L);

long double delta_for_polygon(const Polygon& poly);

}  // namespace task12

