#pragma once
#include <cmath>

namespace task3 {

struct Point {
    long double x = 0.0L;
    long double y = 0.0L;
};

bool point_on_segment(const Point& a, const Point& b,
                      const Point& p, long double eps);

int point_segment_relation(const Point& a, const Point& b,
                           const Point& p, long double eps);

} 
