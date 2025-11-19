#pragma once
#include <QPointF>
#include <task3/point_segment_ld.hpp>

using LDPoint = task3::Point;

class CanvasModel {
public:
    void setA(const LDPoint& a) { a_ = a; hasA_ = true; }
    void setB(const LDPoint& b) { b_ = b; hasB_ = true; }
    void setP(const LDPoint& p) { p_ = p; hasP_ = true; }

    bool hasA() const { return hasA_; }
    bool hasB() const { return hasB_; }
    bool hasP() const { return hasP_; }

    LDPoint A() const { return a_; }
    LDPoint B() const { return b_; }
    LDPoint P() const { return p_; }

    // epsilon в единицах координат
    void setEps(long double e);
    long double eps() const { return eps_; }

    // Геометрия в long double
    int  relation() const;          // 1=LEFT, -1=RIGHT, 0=ON, 999=нехватает данных

    // утилиты для UI
    void clearAB() { hasA_ = hasB_ = false; }
    void clearAll(){ hasA_ = hasB_ = hasP_ = false; }

private:
    LDPoint a_{}, b_{}, p_{};
    bool hasA_ = false, hasB_ = false, hasP_ = false;
    long double eps_ = 1.0L;
};
