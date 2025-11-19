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

Task11Canvas::Task11Canvas(Task11Model& model, QWidget* parent)
    : QWidget(parent), model_(model) {
    setMouseTracking(true);
}

void Task11Canvas::setLive(bool on) {
    live_ = on;
}

double Task11Canvas::clamp(double v, double lo, double hi) const {
    if (v < lo) return lo;
    if (v > hi) return hi;
    return v;
}

QPointF Task11Canvas::screenToWorld(const QPointF& screen) const {
    return (screen - viewOffset_) / viewScale_;
}

QPointF Task11Canvas::worldToScreen(const QPointF& world) const {
    return world * viewScale_ + viewOffset_;
}

double Task11Canvas::pickRadius() const {
    return 12.0 / viewScale_;
}

void Task11Canvas::zoomAt(double factor) {
    const double next = clamp(viewScale_ * factor, kMinScale, kMaxScale);
    if (qFuzzyCompare(next, viewScale_)) return;
    const QPointF screenCenter(width() * 0.5, height() * 0.5);
    const QPointF worldUnder = screenToWorld(screenCenter);
    viewScale_ = next;
    viewOffset_ = screenCenter - viewScale_ * worldUnder;
    update();
}

void Task11Canvas::resetView() {
    viewScale_ = 1.0;
    viewOffset_ = QPointF(0.0, 0.0);
    update();
}

QColor Task11Canvas::queryColor(task11::Region region) const {
    switch (region) {
        case task11::Region::Inside: return QColor(0, 150, 0);
        case task11::Region::Outside: return QColor(200, 50, 50);
        case task11::Region::Boundary: return QColor(230, 180, 40);
        case task11::Region::NearBoundary: return QColor(20, 120, 220);
    }
    return QColor(0, 0, 0);
}

void Task11Canvas::paintEvent(QPaintEvent*) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::white);

    p.save();
    p.scale(viewScale_, viewScale_);
    p.translate(viewOffset_ / viewScale_);

    const double r = 4.0 / viewScale_;
    const double line = 2.0 / viewScale_;
    p.setPen(QPen(Qt::black, line, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));

    const auto& raw = model_.rawPoints();
    if (!model_.hasHull()) {
        for (size_t i = 1; i < raw.size(); ++i) {
            p.drawLine(raw[i - 1], raw[i]);
        }
    }
    for (const auto& point : raw) {
        p.setBrush(Qt::gray);
        p.drawEllipse(point, r, r);
    }

    if (model_.hasHull()) {
        p.setPen(QPen(Qt::darkGreen, line * 1.5, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto& hull = model_.hull();
        for (size_t i = 0; i < hull.size(); ++i) {
            p.drawLine(hull[i], hull[(i + 1) % hull.size()]);
        }
    }

    if (model_.hasQueryPoint()) {
        p.setPen(Qt::NoPen);
        p.setBrush(queryColor(model_.classification().region));
        p.drawEllipse(model_.queryPoint(), r * 1.4, r * 1.4);
    }

    p.restore();
}

void Task11Canvas::mousePressEvent(QMouseEvent* event) {
    const QPointF world = screenToWorld(eventPosition(event));
    if (event->button() == Qt::LeftButton) {
        if (!model_.hasHull()) {
            if (model_.addPoint(world)) {
                emit modelChanged();
                update();
            }
            return;
        }
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

void Task11Canvas::mouseMoveEvent(QMouseEvent* event) {
    if (!dragging_ || !live_) return;
    if (model_.moveQueryPoint(screenToWorld(eventPosition(event)))) {
        emit modelChanged();
        update();
    }
}

void Task11Canvas::mouseReleaseEvent(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) dragging_ = false;
}

void Task11Canvas::mouseDoubleClickEvent(QMouseEvent* event) {
    if (event->button() != Qt::LeftButton) return;
    if (model_.finishHull()) {
        emit modelChanged();
        update();
    }
}

void Task11Canvas::keyPressEvent(QKeyEvent* event) {
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
