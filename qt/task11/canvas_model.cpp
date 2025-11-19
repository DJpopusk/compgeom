#include "canvas_model.h"

#include <algorithm>

namespace {

task11::Point toPoint(const QPointF& p) {
    return task11::Point{static_cast<long double>(p.x()),
                         static_cast<long double>(p.y())};
}

QPointF toQPoint(const task11::Point& p) {
    return QPointF(static_cast<double>(p.x), static_cast<double>(p.y));
}

}  // namespace

void Task11Model::reset() {
    points_.clear();
    hull_.clear();
    phase_ = Task11Phase::Collecting;
    hasQuery_ = false;
    lastClass_ = {};
}

bool Task11Model::addPoint(const QPointF& p) {
    if (phase_ != Task11Phase::Collecting) return false;
    points_.push_back(p);
    return true;
}

bool Task11Model::finishHull() {
    if (points_.size() < 3) return false;
    std::vector<task11::Point> pts;
    pts.reserve(points_.size());
    for (const auto& p : points_) pts.push_back(toPoint(p));
    const auto hull = task11::convex_hull(pts);
    if (hull.size() < 3) return false;
    hull_.clear();
    for (const auto& p : hull) hull_.push_back(toQPoint(p));
    phase_ = Task11Phase::HullReady;
    hasQuery_ = false;
    lastClass_ = {};
    return true;
}

bool Task11Model::setQueryPoint(const QPointF& p) {
    if (!hasHull()) return false;
    queryPoint_ = p;
    hasQuery_ = true;
    updateClassification();
    phase_ = Task11Phase::Query;
    return true;
}

bool Task11Model::moveQueryPoint(const QPointF& p) {
    if (!hasQuery_) return false;
    queryPoint_ = p;
    updateClassification();
    return true;
}

void Task11Model::updateClassification() {
    std::vector<task11::Point> hullPts;
    hullPts.reserve(hull_.size());
    for (const auto& p : hull_) hullPts.push_back(toPoint(p));
    lastClass_ = task11::classify(hullPts, toPoint(queryPoint_));
}

