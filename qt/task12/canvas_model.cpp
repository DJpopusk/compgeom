#include "canvas_model.h"

namespace {

task12::Point toPoint(const QPointF& p) {
    return task12::Point{static_cast<long double>(p.x()),
                         static_cast<long double>(p.y())};
}

}  // namespace

void Task12Model::reset() {
    contours_.clear();
    activeContour_ = -1;
    phase_ = Task12Phase::EditingContour;
    hasQuery_ = false;
    lastClass_ = {};
    startContour(false);
}

bool Task12Model::startContour(bool hole) {
    if (phase_ == Task12Phase::Ready || phase_ == Task12Phase::Query) return false;
    if (hole) {
        bool hasOuter = false;
        for (const auto& c : contours_) if (!c.hole && c.closed) { hasOuter = true; break; }
        if (!hasOuter) return false;
    }
    if (activeContour_ != -1) return false;
    contours_.push_back(Task12Contour{hole, false, {}});
    activeContour_ = static_cast<int>(contours_.size()) - 1;
    phase_ = Task12Phase::EditingContour;
    return true;
}

bool Task12Model::addPoint(const QPointF& p) {
    if (activeContour_ < 0 || activeContour_ >= static_cast<int>(contours_.size())) return false;
    contours_[activeContour_].pts.push_back(p);
    return true;
}

bool Task12Model::closeContour() {
    if (activeContour_ < 0) return false;
    auto& contour = contours_[activeContour_];
    if (contour.pts.size() < 3) return false;
    contour.closed = true;
    activeContour_ = -1;
    phase_ = Task12Phase::Idle;
    return true;
}

bool Task12Model::finalizeShape() {
    bool hasClosed = false;
    for (const auto& c : contours_) {
        if (c.closed) { hasClosed = true; break; }
    }
    if (!hasClosed) return false;
    phase_ = Task12Phase::Ready;
    hasQuery_ = false;
    lastClass_ = {};
    return true;
}

bool Task12Model::setQueryPoint(const QPointF& p) {
    if (phase_ != Task12Phase::Ready && phase_ != Task12Phase::Query) return false;
    queryPoint_ = p;
    hasQuery_ = true;
    updateClassification();
    phase_ = Task12Phase::Query;
    return true;
}

bool Task12Model::moveQueryPoint(const QPointF& p) {
    if (!hasQuery_) return false;
    queryPoint_ = p;
    updateClassification();
    return true;
}

task12::Polygon Task12Model::toPolygon() const {
    task12::Polygon poly;
    for (const auto& c : contours_) {
        if (!c.closed || c.pts.size() < 3) continue;
        task12::Contour contour;
        contour.hole = c.hole;
        contour.vertices.reserve(c.pts.size());
        for (const auto& pt : c.pts) contour.vertices.push_back(toPoint(pt));
        poly.contours.push_back(std::move(contour));
    }
    return poly;
}

void Task12Model::updateClassification() {
    const auto polygon = toPolygon();
    lastClass_ = task12::classify(polygon, toPoint(queryPoint_));
}

