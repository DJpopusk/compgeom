#include "canvas_model.h"

#include <algorithm>

namespace {

double loop_area(const std::vector<QPointF>& pts) {
    if (pts.size() < 3) return 0.0;
    double s = 0.0;
    for (size_t i = 0; i < pts.size(); ++i) {
        const auto& a = pts[i];
        const auto& b = pts[(i + 1) % pts.size()];
        s += a.x() * b.y() - a.y() * b.x();
    }
    return s * 0.5;
}

void ensure_orientation(std::vector<QPointF>& pts, bool positive) {
    const double s = loop_area(pts);
    if ((positive && s < 0) || (!positive && s > 0)) {
        std::reverse(pts.begin(), pts.end());
    }
}

}

PolyBoolModel::PolyBoolModel(const PolyBoolConfig& cfgA,
                             const PolyBoolConfig& cfgB)
    : cfg_{cfgA, cfgB} {
    reset();
}

void PolyBoolModel::reset() {
    polys_[0] = PolyStore{};
    polys_[1] = PolyStore{};
    phase_ = PolyBoolPhase::EditingA;
    mode_ = PolyBoolMode::Intersection;
    result_.clear();
    outlinesA_.clear();
    outlinesB_.clear();
    startContour(false);
}

PolyBoolModel::PolyStore& PolyBoolModel::currentPoly() {
    return phase_ == PolyBoolPhase::EditingB || phase_ == PolyBoolPhase::IdleB
               ? polys_[1]
               : polys_[0];
}

const PolyBoolModel::PolyStore& PolyBoolModel::currentPoly() const {
    return const_cast<PolyBoolModel*>(this)->currentPoly();
}

bool PolyBoolModel::addPointTo(PolyStore& store, const QPointF& p) {
    if (store.active < 0 || store.active >= static_cast<int>(store.contours.size())) {
        return false;
    }
    store.contours[store.active].pts.push_back(p);
    syncOutlines();
    if (phase_ == PolyBoolPhase::Ready) {
        if (!hasClosedA()) phase_ = PolyBoolPhase::IdleA;
        else if (!hasClosedB()) phase_ = PolyBoolPhase::IdleB;
        else rebuildResult();
    }
    return true;
}

bool PolyBoolModel::addPoint(const QPointF& p) {
    return addPointTo(currentPoly(), p);
}

bool PolyBoolModel::canAddHole(int index) const {
    return cfg_[index].allowHoles;
}

bool PolyBoolModel::startContour(bool hole) {
    const int idx = (phase_ == PolyBoolPhase::EditingB || phase_ == PolyBoolPhase::IdleB) ? 1 : 0;
    if (hole && !canAddHole(idx)) return false;
    auto& store = polys_[idx];
    if (store.active != -1) return false;
    if (hole && store.contours.empty()) return false;
    if (!cfg_[idx].allowExtraContours && !store.contours.empty() && !hole) return false;
    store.contours.push_back(Contour{hole, false, {}});
    store.active = static_cast<int>(store.contours.size()) - 1;
    phase_ = (idx == 0) ? PolyBoolPhase::EditingA : PolyBoolPhase::EditingB;
    return true;
}

bool PolyBoolModel::closeContour() {
    auto& store = currentPoly();
    if (store.active < 0) return false;
    auto& contour = store.contours[store.active];
    if (contour.pts.size() < 3) return false;
    contour.closed = true;
    store.active = -1;
    syncOutlines();
    if (phase_ == PolyBoolPhase::EditingA) phase_ = PolyBoolPhase::IdleA;
    else if (phase_ == PolyBoolPhase::EditingB) phase_ = PolyBoolPhase::IdleB;
    if (phase_ == PolyBoolPhase::Ready) {
        if (!hasClosedB()) phase_ = PolyBoolPhase::IdleB;
        else rebuildResult();
    }
    return true;
}

bool PolyBoolModel::startSecondPolygon() {
    if (!hasClosedA()) return false;
    if (phase_ != PolyBoolPhase::IdleA) return false;
    phase_ = PolyBoolPhase::EditingB;
    return startContour(false);
}

bool PolyBoolModel::finishSecondPolygon() {
    if (!hasClosedA() || !hasClosedB()) return false;
    if (phase_ != PolyBoolPhase::IdleB) return false;
    phase_ = PolyBoolPhase::Ready;
    rebuildResult();
    return true;
}

bool PolyBoolModel::deletePoint(const QPointF& p, double radius) {
    VertexRef ref;
    if (!beginDrag(p, radius, ref)) return false;
    auto& poly = polys_[ref.poly];
    auto& contour = poly.contours[ref.contour];
    contour.pts.erase(contour.pts.begin() + ref.index);
    if (contour.pts.size() < 3) contour.closed = false;
    syncOutlines();
    if (phase_ == PolyBoolPhase::Ready) {
        if (!hasClosedA()) phase_ = PolyBoolPhase::IdleA;
        else if (!hasClosedB()) phase_ = PolyBoolPhase::IdleB;
        else rebuildResult();
    }
    return true;
}

bool PolyBoolModel::beginDrag(const QPointF& p, double radius, VertexRef& ref) const {
    double best = radius * radius;
    VertexRef found;
    auto check = [&](int polyIndex, const PolyStore& store) {
        for (size_t ci = 0; ci < store.contours.size(); ++ci) {
            const auto& contour = store.contours[ci];
            for (size_t vi = 0; vi < contour.pts.size(); ++vi) {
                const QPointF& v = contour.pts[vi];
                const double dx = v.x() - p.x();
                const double dy = v.y() - p.y();
                const double d2 = dx * dx + dy * dy;
                if (d2 <= best) {
                    best = d2;
                    found = VertexRef{polyIndex, static_cast<int>(ci), static_cast<int>(vi)};
                }
            }
        }
    };
    check(0, polys_[0]);
    check(1, polys_[1]);
    if (!found.valid()) return false;
    ref = found;
    activeHandle_ = found;
    return true;
}

bool PolyBoolModel::moveVertex(const VertexRef& ref, const QPointF& p) {
    if (!ref.valid()) return false;
    auto& contour = polys_[ref.poly].contours[ref.contour];
    if (ref.index < 0 || ref.index >= static_cast<int>(contour.pts.size())) return false;
    contour.pts[ref.index] = p;
    syncOutlines();
    if (phase_ == PolyBoolPhase::Ready) {
        if (!hasClosedA()) phase_ = PolyBoolPhase::IdleA;
        else if (!hasClosedB()) phase_ = PolyBoolPhase::IdleB;
        else rebuildResult();
    }
    return true;
}

bool PolyBoolModel::hasClosedA() const {
    for (const auto& c : polys_[0].contours) {
        if (c.closed && c.pts.size() >= 3) return true;
    }
    return false;
}

bool PolyBoolModel::hasClosedB() const {
    for (const auto& c : polys_[1].contours) {
        if (c.closed && c.pts.size() >= 3) return true;
    }
    return false;
}

void PolyBoolModel::syncOutlines() {
    outlinesA_.clear();
    outlinesB_.clear();
    auto fill = [](const PolyStore& store, std::vector<Outline>& dst) {
        for (const auto& c : store.contours) {
            Outline outline;
            outline.pts = c.pts;
            outline.closed = c.closed;
            outline.hole = c.hole;
            dst.push_back(outline);
        }
    };
    fill(polys_[0], outlinesA_);
    fill(polys_[1], outlinesB_);
}

task10::Loop PolyBoolModel::makeLoop(const Contour& contour) {
    task10::Loop loop;
    loop.hole = contour.hole;
    loop.vertices.reserve(contour.pts.size());
    std::vector<QPointF> pts = contour.pts;
    if (!pts.empty()) {
        ensure_orientation(pts, !contour.hole);
    }
    for (const auto& p : pts) {
        loop.vertices.push_back(task10::Point{p.x(), p.y()});
    }
    return loop;
}

task10::Polygon PolyBoolModel::makePolygon(const PolyStore& poly) const {
    task10::Polygon pg;
    for (const auto& contour : poly.contours) {
        if (!contour.closed || contour.pts.size() < 3) continue;
        pg.loops.push_back(makeLoop(contour));
    }
    return pg;
}

void PolyBoolModel::rebuildResult() {
    const auto pa = makePolygon(polys_[0]);
    const auto pb = makePolygon(polys_[1]);
    task10::Operation op = task10::Operation::Intersection;
    if (mode_ == PolyBoolMode::Union) op = task10::Operation::Union;
    else if (mode_ == PolyBoolMode::Difference) op = task10::Operation::DifferenceAB;
    const auto polys = task10::boolean_operation(pa, pb, op);
    result_.clear();
    for (const auto& poly : polys) {
        for (const auto& loop : poly.loops) {
            ResultLoop r;
            r.hole = loop.hole;
            for (const auto& p : loop.vertices) {
                r.points.push_back(QPointF(p.x, p.y));
            }
            result_.push_back(std::move(r));
        }
    }
}

void PolyBoolModel::setMode(PolyBoolMode mode) {
    mode_ = mode;
    if (phase_ == PolyBoolPhase::Ready) rebuildResult();
}
