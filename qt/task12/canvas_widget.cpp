#include "canvas_widget.h"

#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>

namespace {
constexpr double kZoomFactor = 1.2;
constexpr double kMinScale = 0.2;
constexpr double kMaxScale = 40.0;
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
QPointF eventPosition(const QMouseEvent* e) { return e->position(); }
#else
QPointF eventPosition(const QMouseEvent* e) { return e->posF(); }
#endif
}  // namespace

Task12Canvas::Task12Canvas(Task12Model& model, QWidget* parent)
    : QWidget(parent), model_(model) {
    setMouseTracking(true);
}

void Task12Canvas::setLive(bool on) {
    live_ = on;
}

double Task12Canvas::clamp(double v, double lo, double hi) const {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

QPointF Task12Canvas::screenToWorld(const QPointF& screen) const {
    return (screen - viewOffset_) / viewScale_;
}

QPointF Task12Canvas::worldToScreen(const QPointF& world) const {
    return world * viewScale_ + viewOffset_;
}

double Task12Canvas::pickRadius() const {
    return 12.0 / viewScale_;
}

void Task12Canvas::zoomAt(double factor) {
    const double next = clamp(viewScale_ * factor, kMinScale, kMaxScale);
    if (qFuzzyCompare(next, viewScale_)) return;
    const QPointF screenCenter(width() * 0.5, height() * 0.5);
    const QPointF worldUnder = screenToWorld(screenCenter);
    viewScale_ = next;
    viewOffset_ = screenCenter - viewScale_ * worldUnder;
    update();
}

void Task12Canvas::resetView() {
    viewScale_ = 1.0;
    viewOffset_ = QPointF(0.0, 0.0);
    update();
}

QColor Task12Canvas::queryColor(task12::Region region) const {
    switch (region) {
        case task12::Region::Inside: return QColor(0, 150, 0);
        case task12::Region::Outside: return QColor(200, 50, 50);
        case task12::Region::Boundary: return QColor(230, 180, 40);
        case task12::Region::NearBoundary: return QColor(20, 120, 220);
    }
    return QColor(0, 0, 0);
}

void Task12Canvas::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::white);

    p.save();
    p.scale(viewScale_, viewScale_);
    p.translate(viewOffset_ / viewScale_);

    const double r = 4.0 / viewScale_;
    const double line = 2.0 / viewScale_;

    for (const auto& contour : model_.contours()) {
        if (contour.pts.empty()) continue;
        QColor color = contour.hole ? QColor(130, 0, 150) : QColor(0, 120, 0);
        p.setPen(QPen(color, line, contour.hole ? Qt::DashLine : Qt::SolidLine,
                      Qt::RoundCap, Qt::RoundJoin));
        for (size_t i = 1; i < contour.pts.size(); ++i) {
            p.drawLine(contour.pts[i - 1], contour.pts[i]);
        }
        if (contour.closed && contour.pts.size() >= 2) {
            p.drawLine(contour.pts.back(), contour.pts.front());
        }
        p.setPen(Qt::NoPen);
        p.setBrush(color.lighter(150));
        for (const auto& pt : contour.pts) {
            p.drawEllipse(pt, r, r);
        }
    }

    if (model_.hasQueryPoint()) {
        p.setPen(Qt::NoPen);
        p.setBrush(queryColor(model_.classification().region));
        p.drawEllipse(model_.queryPoint(), r * 1.4, r * 1.4);
    }

    p.restore();
}

void Task12Canvas::mousePressEvent(QMouseEvent* event) {
    const QPointF world = screenToWorld(eventPosition(event));
    if (event->button() == Qt::LeftButton) {
        if (model_.editingActive()) {
            if (model_.addPoint(world)) {
                emit modelChanged();
                update();
            }
            return;
        }
        if (model_.phase() == Task12Phase::Ready || model_.phase() == Task12Phase::Query) {
            if (model_.hasQueryPoint()) {
                const QPointF qp = model_.queryPoint();
                const double dx = qp.x() - world.x();
                const double dy = qp.y() - world.y();
                if (std::sqrt(dx * dx + dy * dy) <= pickRadius()) {
                    dragging_ = true;
                    return;
                }
            }
            if (model_.setQueryPoint(world)) {
                emit modelChanged();
                update();
            }
        }
    }
}

void Task12Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (!dragging_ || !live_) return;
    if (model_.moveQueryPoint(screenToWorld(eventPosition(event)))) {
        emit modelChanged();
        update();
    }
}

void Task12Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) dragging_ = false;
}

void Task12Canvas::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    if (model_.closeContour()) {
        emit modelChanged();
        update();
    }
}

void Task12Canvas::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
        zoomAt(kZoomFactor);
        event->accept();
        return;
    }
    if (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore) {
        zoomAt(1.0 / kZoomFactor);
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
