#pragma once
#include <vector>
#include <QPointF>
#include <task5/delaunay.hpp>

using LDPoint = task5::Point;
using Triangle = task5::Triangle;

class CanvasModel {
public:
    
    int  addPoint(const LDPoint& p);               
    void setPoint(int idx, const LDPoint& p);      
    const std::vector<LDPoint>& points() const { return pts_; }
    void clear();

    
    bool computeDelaunay();

    
    const std::vector<Triangle>& triangles() const { return tris_; }
    bool hasTriangulation() const { return !tris_.empty(); }

    
    size_t size() const { return pts_.size(); }

private:
    std::vector<LDPoint> pts_;
    std::vector<Triangle> tris_;
};
