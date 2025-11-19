#pragma once
#include <vector>

namespace task789 {

struct Point {
    long double x = 0.0L;
    long double y = 0.0L;
};

using Polygon = std::vector<Point>;

Polygon convex_hull(const std::vector<Point>& pts);

enum class Operation { Intersection, Union, DifferenceAB };

std::vector<Polygon> boolean_operation(const Polygon& hullA,
                                       const Polygon& hullB,
                                       Operation op);

} // namespace task789
