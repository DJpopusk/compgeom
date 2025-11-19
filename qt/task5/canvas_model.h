#pragma once
#include <vector>
#include <QPointF>
#include <task5/delaunay.hpp>

using LDPoint = task5::Point;
using Triangle = task5::Triangle;

class CanvasModel {
public:
    // управление набором точек
    int  addPoint(const LDPoint& p);               // возвращает индекс
    void setPoint(int idx, const LDPoint& p);      // перемещение существующей точки
    const std::vector<LDPoint>& points() const { return pts_; }
    void clear();

    // триангуляция Делоне (Bowyer–Watson)
    bool computeDelaunay();

    // результат
    const std::vector<Triangle>& triangles() const { return tris_; }
    bool hasTriangulation() const { return !tris_.empty(); }

    // утилиты
    size_t size() const { return pts_.size(); }

private:
    std::vector<LDPoint> pts_;
    std::vector<Triangle> tris_;
};
