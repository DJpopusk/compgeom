#pragma once

#include <vector>

namespace task10 {

struct Point {
    double x = 0.0;
    double y = 0.0;
};

struct Loop {
    bool hole = false;
    std::vector<Point> vertices;
};

struct Polygon {
    std::vector<Loop> loops;
};

enum class Operation { Intersection, Union, DifferenceAB };

std::vector<Polygon> boolean_operation(const Polygon& a,
                                       const Polygon& b,
                                       Operation op);

}
