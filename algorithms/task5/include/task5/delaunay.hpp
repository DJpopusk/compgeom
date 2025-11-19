#pragma once
#include <vector>

namespace task5 {

struct Point {
    long double x = 0.0L;
    long double y = 0.0L;
};

struct Triangle {
    int a = -1;
    int b = -1;
    int c = -1;
};

bool delaunay_triangulation(const std::vector<Point>& pts,
                            std::vector<Triangle>* triangles);

} 
