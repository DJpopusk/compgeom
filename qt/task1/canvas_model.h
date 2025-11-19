#pragma once
#include <QPointF>

class CanvasModel {
public:
    
    void setA(const QPointF& a) { p1_ = a; hasP1_ = true; }
    void setB(const QPointF& b) { p2_ = b; hasP2_ = true; }
    void setP(const QPointF& p) { pt_ = p; hasPt_ = true; }
    void clearB()               { hasP2_ = false; }
    void clearAll()             { hasP1_ = hasP2_ = hasPt_ = false; }

    
    void setEps(double e);
    double eps() const { return onEps_; }

    
    bool hasA() const { return hasP1_; }
    bool hasB() const { return hasP2_; }
    bool hasP() const { return hasPt_; }
    const QPointF& A() const { return p1_; }
    const QPointF& B() const { return p2_; }
    const QPointF& P() const { return pt_; }

    
    int  relation() const; 

private:
    QPointF p1_{}, p2_{}, pt_{};
    bool hasP1_ = false, hasP2_ = false, hasPt_ = false;
    double onEps_ = 1.0;
};
