#pragma once
#include <vector>

namespace task4 {

struct Point {
    long double x = 0.0L;
    long double y = 0.0L;
};

using HullIndices = std::vector<int>;

bool convex_hull_indices(const std::vector<Point>& pts, HullIndices* hull);

} // namespace task4
