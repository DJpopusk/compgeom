#include "canvas_model.h"

#include <limits>

namespace {
long double dist2(const LDPoint& a, const QPointF& w) {
    const long double dx = a.x - static_cast<long double>(w.x());
    const long double dy = a.y - static_cast<long double>(w.y());
    return dx*dx + dy*dy;
}
}

LDPoly* CanvasModel::activePoints() {
    if (phase_ == Phase::EditingA) return &ptsA_;
    if (phase_ == Phase::EditingB) return &ptsB_;
    return nullptr;
}
const LDPoly* CanvasModel::activePoints() const {
    if (phase_ == Phase::EditingA) return &ptsA_;
    if (phase_ == Phase::EditingB) return &ptsB_;
    return nullptr;
}

task789::Operation CanvasModel::currentOperation() const {
    switch (mode_) {
    case OpMode::Intersection: return task789::Operation::Intersection;
    case OpMode::Union:        return task789::Operation::Union;
    case OpMode::DiffAminusB:  return task789::Operation::DifferenceAB;
    }
    return task789::Operation::Intersection;
}

void CanvasModel::startB() {
    if (phase_ != Phase::EditingA) return;
    phase_ = Phase::EditingB;
    recompute();
}

void CanvasModel::finish() {
    if (phase_ != Phase::EditingB) return;
    phase_ = Phase::Done;
    recompute();
}

int CanvasModel::addPoint(const QPointF& w) {
    auto* pts = activePoints();
    if (!pts) return -1;
    pts->push_back(toLD(w));
    recompute();
    return static_cast<int>(pts->size()) - 1;
}

int CanvasModel::pickPointIndex(const QPointF& w, double pickRadiusWorld) const {
    const auto* pts = activePoints();
    if (!pts) return -1;
    const long double pick2 = static_cast<long double>(pickRadiusWorld * pickRadiusWorld);
    int best = -1;
    long double bestd = std::numeric_limits<long double>::infinity();
    for (int i=0;i<static_cast<int>(pts->size());++i) {
        const long double d2 = dist2((*pts)[i], w);
        if (d2 <= pick2 && d2 < bestd) {
            bestd = d2;
            best = i;
        }
    }
    return best;
}

void CanvasModel::movePoint(int idx, const QPointF& w) {
    auto* pts = activePoints();
    if (!pts) return;
    if (idx < 0 || idx >= static_cast<int>(pts->size())) return;
    (*pts)[idx] = toLD(w);
    recompute();
}

void CanvasModel::deletePoint(int idx) {
    auto* pts = activePoints();
    if (!pts) return;
    if (idx < 0 || idx >= static_cast<int>(pts->size())) return;
    pts->erase(pts->begin() + idx);
    recompute();
}

void CanvasModel::setOpMode(OpMode m) {
    mode_ = m;
    if (phase_ == Phase::Done) recompute();
}

void CanvasModel::recompute() {
    hullA_ = task789::convex_hull(ptsA_);
    hullB_ = task789::convex_hull(ptsB_);

    if (phase_ == Phase::Done && !hullA_.empty() && !hullB_.empty()) {
        result_.polys = task789::boolean_operation(hullA_, hullB_, currentOperation());
    } else {
        result_.polys.clear();
    }
}
