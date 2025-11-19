#include "canvas_model.h"

#include <task1/point_segment.hpp>
#include <cmath>

namespace {
task1::Vec2 toVec(const QPointF& p) {
    return task1::Vec2{p.x(), p.y()};
}
}

void CanvasModel::setEps(double e) {
    if (e < 0.0) e = 0.0;
    if (e > 1000.0) e = 1000.0;
    onEps_ = e;
}

int CanvasModel::relation() const {
    if (!hasP1_ || !hasP2_ || !hasPt_) return 999;

    return task1::point_segment_relation(
        toVec(p1_), toVec(p2_), toVec(pt_), onEps_);
}
