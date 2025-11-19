#include "canvas_model.h"

int CanvasModel::addPoint(const LDPoint& p) {
    pts_.push_back(p);
    return static_cast<int>(pts_.size()) - 1;
}

void CanvasModel::setPoint(int idx, const LDPoint& p) {
    if (idx < 0 || idx >= static_cast<int>(pts_.size())) return;
    pts_[idx] = p;
}

void CanvasModel::clear() {
    pts_.clear();
    tris_.clear();
}

bool CanvasModel::computeDelaunay() {
    return task5::delaunay_triangulation(pts_, &tris_);
}
