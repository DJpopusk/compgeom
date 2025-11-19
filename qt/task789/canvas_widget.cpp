#include "canvas_widget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QPainterPath>
#include <QtMath>

CanvasWidget::CanvasWidget(CanvasModel& model, QWidget *parent)
    : QWidget(parent), model_(model)
{
    setMouseTracking(true);
}

void CanvasWidget::setLive(bool on) {
    live_ = on;
    if (live_) model_.recompute();
    updateStatus();
    update();
}

void CanvasWidget::actionStartB() {
    if (model_.phase() == Phase::EditingA) {
        model_.startB();
        if (live_) model_.recompute();
        updateStatus();
        update();
    }
}
void CanvasWidget::actionFinish() {
    if (model_.phase() == Phase::EditingB) {
        model_.finish();
        updateStatus();
        update();
    }
}

double CanvasWidget::dclamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
QPointF CanvasWidget::mousePointF(const QMouseEvent *e) { return e->position(); }
#else
QPointF CanvasWidget::mousePointF(const QMouseEvent *e) { return e->pos(); }
#endif

QPointF CanvasWidget::screenToWorld(const QPointF &screen) const {
    return (screen - viewOffset_) / viewScale_;
}
QPointF CanvasWidget::worldToScreen(const QPointF &world) const {
    return world * viewScale_ + viewOffset_;
}
void CanvasWidget::zoomAtCenter(double factor) {
    const double old = viewScale_;
    const double next = dclamp(old * factor, 0.1, 50.0);
    if (qFuzzyCompare(old, next)) return;

    const QPointF screenCenter(width()/2.0, height()/2.0);
    const QPointF worldUnder = screenToWorld(screenCenter);

    viewScale_ = next;
    viewOffset_ = screenCenter - viewScale_ * worldUnder;
    update();
}
void CanvasWidget::resetView() {
    viewScale_ = 1.0;
    viewOffset_ = QPointF(0, 0);
    update();
}

int CanvasWidget::pickActiveIndexScreen(const QPointF& m) const {
    const double pickPx = 10.0;
    const double pickWorld = pickPx / viewScale_;
    return model_.pickPointIndex(screenToWorld(m), pickWorld);
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    const QPointF m = mousePointF(event);
    if (event->button() == Qt::LeftButton) {
        if (live_) {
            dragIndex_ = pickActiveIndexScreen(m);
            if (dragIndex_ != -1) { dragging_ = true; return; }
        }
        
        model_.addPoint(screenToWorld(m));
        if (live_) model_.recompute();
        updateStatus();
        update();
    } else if (event->button() == Qt::RightButton) {
        int idx = pickActiveIndexScreen(m);
        if (idx != -1) {
            model_.deletePoint(idx);
            if (live_) model_.recompute();
            updateStatus();
            update();
        }
    }
}

void CanvasWidget::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    if (model_.phase() == Phase::EditingA) actionStartB();
    else if (model_.phase() == Phase::EditingB) actionFinish();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!live_ || !dragging_) return;
    const QPointF w = screenToWorld(mousePointF(event));
    if (dragIndex_ >= 0) {
        model_.movePoint(dragIndex_, w);
        updateStatus();
        update();
    }
}
void CanvasWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (!live_) return;
    if (event->button() == Qt::LeftButton && dragging_) {
        dragging_ = false;
        dragIndex_ = -1;
    }
}

void CanvasWidget::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Plus || event->key() == Qt::Key_Equal) {
        zoomAtCenter(1.2); event->accept(); return;
    }
    if (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore) {
        zoomAtCenter(1.0/1.2); event->accept(); return;
    }
    if (event->key() == Qt::Key_0) {
        resetView(); event->accept(); return;
    }
    QWidget::keyPressEvent(event);
}

void CanvasWidget::updateStatus() {
    QString ph = (model_.phase()==Phase::EditingA) ? "Редактирование A"
                 : (model_.phase()==Phase::EditingB) ? "Редактирование B"
                                                       : "Готово";
    emit statusText(ph + QString(" | points A: %1  B: %2")
                             .arg((int)model_.pointsA().size())
                             .arg((int)model_.pointsB().size()));
}

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::white);

    p.save();
    p.scale(viewScale_, viewScale_);
    p.translate(viewOffset_ / viewScale_);

    const double lineW   = 2.5 / viewScale_;
    const double radMark = 5.0 / viewScale_;

    auto drawPoints = [&](const LDPoly& pts){
        p.setPen(QPen(Qt::black, lineW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        for (const auto& q : pts) {
            QPointF v((double)q.x, (double)q.y);
            p.drawEllipse(v, radMark, radMark);
        }
    };
    auto drawPoly = [&](const LDPoly& poly, const QColor& col, double w){
        if (poly.size()<2) return;
        p.setPen(QPen(col, w / viewScale_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        for (int i=0;i<(int)poly.size();++i) {
            QPointF A((double)poly[i].x, (double)poly[i].y);
            QPointF B((double)poly[(i+1)%poly.size()].x, (double)poly[(i+1)%poly.size()].y);
            p.drawLine(A,B);
        }
    };

    
    drawPoints(model_.pointsA());
    drawPoints(model_.pointsB());

    
    if (!model_.hullA().empty()) drawPoly(model_.hullA(), QColor(0,170,0), 3.0);
    if (!model_.hullB().empty()) drawPoly(model_.hullB(), QColor(160,0,200), 3.0);

    
    if (model_.phase() == Phase::Done) {
        const auto& R = model_.result().polys;
        if (!R.empty()) {
            QColor fill(20,120,220,70);
            QPainterPath path;
            path.setFillRule(Qt::OddEvenFill);
            for (const auto& poly : R) {
                if (poly.size() < 3) continue;
                QPolygonF q; q.reserve((int)poly.size());
                for (auto &pt : poly) q << QPointF((double)pt.x, (double)pt.y);
                path.addPolygon(q);
            }
            p.fillPath(path, fill);
        }
    }

    p.restore();}
