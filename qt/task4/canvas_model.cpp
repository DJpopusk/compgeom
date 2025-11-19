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
    hull_.clear();
}

bool CanvasModel::computeHull() {
    if (!task4::convex_hull_indices(pts_, &hull_)) {
        hull_.clear();
        return false;
    }
    return true;
}
