#include "canvas_model.h"

void CanvasModel::setEps(long double e) {
    if (e < 0.0L) e = 0.0L;
    if (e > 1e9L) e = 1e9L;
    eps_ = e;
}

int CanvasModel::relation() const {
    if (!hasA_ || !hasB_ || !hasP_) return 999;

    return task3::point_segment_relation(a_, b_, p_, eps_);
}
