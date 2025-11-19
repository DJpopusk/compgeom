#include "canvas_widget.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

namespace {
constexpr double kZoomFactor = 1.2;
constexpr double kMinScale = 0.2;
constexpr double kMaxScale = 40.0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QPointF eventPos(const QMouseEvent* e) { return e->position(); }
#else
QPointF eventPos(const QMouseEvent* e) { return e->posF(); }
#endif
}

PolyBoolCanvas::PolyBoolCanvas(PolyBoolModel& model, QWidget* parent)
    : QWidget(parent), model_(model) {
    setMouseTracking(true);
}

void PolyBoolCanvas::setLive(bool on) {
    live_ = on;
}

double PolyBoolCanvas::clamp(double v, double lo, double hi) const {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

QPointF PolyBoolCanvas::screenToWorld(const QPointF& screen) const {
    return (screen - viewOffset_) / viewScale_;
}

QPointF PolyBoolCanvas::worldToScreen(const QPointF& world) const {
    return world * viewScale_ + viewOffset_;
}

double PolyBoolCanvas::pickRadiusWorld() const {
    return 10.0 / viewScale_;
}

void PolyBoolCanvas::zoomAtCenter(double factor) {
    const double next = clamp(viewScale_ * factor, kMinScale, kMaxScale);
    if (qFuzzyCompare(next, viewScale_)) return;
    const QPointF screenCenter(width() * 0.5, height() * 0.5);
    const QPointF worldUnder = screenToWorld(screenCenter);
    viewScale_ = next;
    viewOffset_ = screenCenter - viewScale_ * worldUnder;
    update();
}

void PolyBoolCanvas::resetView() {
    viewScale_ = 1.0;
    viewOffset_ = QPointF(0.0, 0.0);
    update();
}

void PolyBoolCanvas::notifyChange() {
    emit canvasChanged();
    update();
}

void PolyBoolCanvas::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::white);

    p.save();
    p.scale(viewScale_, viewScale_);
    p.translate(viewOffset_ / viewScale_);

    const double lineW = 2.0 / viewScale_;
    const double pointR = 4.0 / viewScale_;

    auto drawPoly = [&](const std::vector<PolyBoolModel::Outline>& outlines,
                        const QColor& color) {
        if (outlines.empty()) return;
        p.setPen(QPen(color, lineW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        for (const auto& outline : outlines) {
            const auto& loop = outline.pts;
            if (loop.size() < 2) continue;
            for (size_t i = 0; i < loop.size() - 1; ++i) {
                p.drawLine(loop[i], loop[i + 1]);
            }
            if (outline.closed && loop.size() >= 2) p.drawLine(loop.back(), loop.front());
        }
        p.setPen(Qt::NoPen);
        p.setBrush(color.lighter(150));
        for (const auto& outline : outlines) {
            for (const auto& pt : outline.pts) {
                p.drawEllipse(pt, pointR, pointR);
            }
        }
    };

    drawPoly(model_.pointsA(), QColor(0, 120, 0));
    drawPoly(model_.pointsB(), QColor(130, 0, 150));

    if (model_.phase() == PolyBoolPhase::Ready) {
        QPainterPath path;
        path.setFillRule(Qt::OddEvenFill);
        for (const auto& loop : model_.result()) {
            if (loop.points.size() < 3) continue;
            QPolygonF poly;
            for (const auto& pt : loop.points) poly << pt;
            path.addPolygon(poly);
        }
        p.setBrush(QColor(30, 130, 220, 90));
        p.setPen(Qt::NoPen);
        p.fillPath(path, p.brush());
    }

    p.restore();
}

void PolyBoolCanvas::mousePressEvent(QMouseEvent* event) {
    const QPointF world = screenToWorld(eventPos(event));
    if (event->button() == Qt::LeftButton) {
        VertexRef ref;
        if (model_.beginDrag(world, pickRadiusWorld(), ref)) {
            dragging_ = true;
            dragRef_ = ref;
            return;
        }
        if (model_.phase() == PolyBoolPhase::EditingA ||
            model_.phase() == PolyBoolPhase::EditingB) {
            if (model_.addPoint(world)) notifyChange();
        }
        return;
    }
    if (event->button() == Qt::RightButton) {
        if (model_.deletePoint(world, pickRadiusWorld())) notifyChange();
    }
}

void PolyBoolCanvas::mouseMoveEvent(QMouseEvent* event) {
    if (!dragging_) return;
    if (!live_) return;
    const QPointF world = screenToWorld(eventPos(event));
    if (model_.moveVertex(dragRef_, world)) notifyChange();
}

void PolyBoolCanvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton && dragging_) {
        dragging_ = false;
    }
}

void PolyBoolCanvas::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    if (model_.closeContour()) {
        notifyChange();
        return;
    }
    if (model_.phase() == PolyBoolPhase::IdleA) {
        if (model_.startSecondPolygon()) notifyChange();
        return;
    }
    if (model_.phase() == PolyBoolPhase::IdleB) {
        if (model_.finishSecondPolygon()) notifyChange();
    }
}

void PolyBoolCanvas::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
        zoomAtCenter(kZoomFactor);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore) {
        zoomAtCenter(1.0 / kZoomFactor);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_0) {
        resetView();
        event->accept();
        return;
    }
    QWidget::keyPressEvent(event);
}
