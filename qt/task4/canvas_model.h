#pragma once
#include <vector>
#include <QPointF>
#include <task4/convex_hull.hpp>

using LDPoint = task4::Point;

class CanvasModel {
public:
    
    int  addPoint(const LDPoint& p);                  
    void setPoint(int idx, const LDPoint& p);         
    const std::vector<LDPoint>& points() const { return pts_; }
    void clear();

    
    
    bool computeHull();

    
    const std::vector<int>& hullIndices() const { return hull_; }
    bool hasHull() const { return !hull_.empty(); }

    
    size_t size() const { return pts_.size(); }

private:
    std::vector<LDPoint> pts_;
    std::vector<int> hull_; 
};
