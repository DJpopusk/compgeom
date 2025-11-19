#pragma once
#include <QPointF>
#include <optional>

class CanvasModel {
public:
    enum PointId { A=0, B=1, C=2, D=3 };

    // state setters
    void setPoint(PointId id, const QPointF& p);
    const QPointF& point(PointId id) const;
    bool has(PointId id) const;
    void clearAll();

    // создание по кликам (A->B->C->D->A...)
    void placeNext(const QPointF& p);

    // конфиг/ε
    void setEps(double e);
    double eps() const { return eps_; }

    // готовность (все 4 точки поставлены)
    bool ready() const;

    // геометрия
    // true & out=intersection, если отрезки AB и CD пересекаются (с учётом eps)
    bool segmentIntersection(QPointF* out) const;

private:
    QPointF pts_[4] {};
    bool has_[4] {false,false,false,false};
    double eps_ = 1.0;
    int nextIdx_ = 0; // 0..3
};
