#pragma once
#include <QPointF>
#include <optional>

class CanvasModel {
public:
    enum PointId { A=0, B=1, C=2, D=3 };

    
    void setPoint(PointId id, const QPointF& p);
    const QPointF& point(PointId id) const;
    bool has(PointId id) const;
    void clearAll();

    
    void placeNext(const QPointF& p);

    
    void setEps(double e);
    double eps() const { return eps_; }

    
    bool ready() const;

    
    
    bool segmentIntersection(QPointF* out) const;

private:
    QPointF pts_[4] {};
    bool has_[4] {false,false,false,false};
    double eps_ = 1.0;
    int nextIdx_ = 0; 
};
