#include "canvas_model.h"

#include <task2/segment_intersection.hpp>

void CanvasModel::setPoint(PointId id, const QPointF& p) {
    pts_[id] = p;
    has_[id] = true;
}
const QPointF& CanvasModel::point(PointId id) const { return pts_[id]; }
bool CanvasModel::has(PointId id) const { return has_[id]; }

void CanvasModel::clearAll() {
    for (int i=0;i<4;++i) has_[i]=false;
    nextIdx_ = 0;
}

void CanvasModel::placeNext(const QPointF& p) {
    setPoint(static_cast<PointId>(nextIdx_), p);
    nextIdx_ = (nextIdx_+1) % 4;
}

void CanvasModel::setEps(double e) {
    if (e < 0.0) e = 0.0;
    if (e > 1000.0) e = 1000.0;
    eps_ = e;
}

bool CanvasModel::ready() const {
    return has_[A] && has_[B] && has_[C] && has_[D];
}
namespace {
task2::Vec2 toVec(const QPointF& p) {
    return task2::Vec2{p.x(), p.y()};
}
} 

bool CanvasModel::segmentIntersection(QPointF* out) const {
    if (!ready()) return false;

    task2::Vec2 inter;
    task2::Vec2* target = out ? &inter : nullptr;

    const bool res = task2::segment_intersection(
        toVec(pts_[A]), toVec(pts_[B]),
        toVec(pts_[C]), toVec(pts_[D]),
        eps_, target);

    if (res && out) {
        *out = QPointF(inter.x, inter.y);
    }
    return res;
}
