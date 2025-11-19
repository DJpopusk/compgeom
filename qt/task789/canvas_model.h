#pragma once
#include <QPointF>
#include <task789/convex_boolean.hpp>

#include <vector>

using LDPoint = task789::Point;
using LDPoly  = std::vector<LDPoint>;

enum class Phase {
    EditingA,
    EditingB,
    Done
};

enum class OpMode { Intersection, Union, DiffAminusB };

class CanvasModel {
public:
    Phase phase() const { return phase_; }
    void  startB();
    void  finish();

    int  addPoint(const QPointF& w);
    int  pickPointIndex(const QPointF& w, double pickRadiusWorld) const;
    void movePoint(int idx, const QPointF& w);
    void deletePoint(int idx);

    const LDPoly& pointsA() const { return ptsA_; }
    const LDPoly& pointsB() const { return ptsB_; }
    const LDPoly& hullA() const { return hullA_; }
    const LDPoly& hullB() const { return hullB_; }

    void   setOpMode(OpMode m);
    OpMode opMode() const { return mode_; }

    struct Result { std::vector<LDPoly> polys; };
    const Result& result() const { return result_; }

    void recompute();

    static QPointF toQt(const LDPoint& p) { return QPointF(static_cast<double>(p.x), static_cast<double>(p.y)); }
    static LDPoint toLD(const QPointF& p) { return LDPoint{static_cast<long double>(p.x()), static_cast<long double>(p.y())}; }

private:
    LDPoly* activePoints();
    const LDPoly* activePoints() const;

    task789::Operation currentOperation() const;

private:
    Phase  phase_ = Phase::EditingA;
    OpMode mode_  = OpMode::Intersection;

    LDPoly ptsA_;
    LDPoly ptsB_;
    LDPoly hullA_;
    LDPoly hullB_;

    Result result_;
};
