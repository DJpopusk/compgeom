#pragma once
#include <vector>
#include <QPointF>
#include <task4/convex_hull.hpp>

using LDPoint = task4::Point;

class CanvasModel {
public:
    // управление набором точек
    int  addPoint(const LDPoint& p);                  // возвращает индекс
    void setPoint(int idx, const LDPoint& p);         // перемещение существующей точки
    const std::vector<LDPoint>& points() const { return pts_; }
    void clear();

    // построение выпуклой оболочки (монотоная цепь Эндрю)
    // Возвращает true, если оболочка успешно посчитана хоть из 1-2 точек.
    bool computeHull();

    // текущая (последняя посчитанная) оболочка как индексы исходных точек в pts_
    const std::vector<int>& hullIndices() const { return hull_; }
    bool hasHull() const { return !hull_.empty(); }

    // утилиты
    size_t size() const { return pts_.size(); }

private:
    std::vector<LDPoint> pts_;
    std::vector<int> hull_; // индексы в pts_
};
