#include "canvas_widget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QtMath>
#include <limits>

CanvasWidget::CanvasWidget(CanvasModel& model, QWidget *parent)
    : QWidget(parent), model_(model)
{
    setMouseTracking(true);
}

void CanvasWidget::setLive(bool on) {
    live_ = on;
    if (live_) {
        showComputedOnce_ = false;
        model_.computeHull();
        emit hullStatus((int)model_.size(), (int)model_.hullIndices().size());
    }
    update();
}

void CanvasWidget::computeOnce() {
    showComputedOnce_ = model_.computeHull();
    emit hullStatus((int)model_.size(), (int)model_.hullIndices().size());
    update();
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

int CanvasWidget::pickPointIndex(const QPointF& mouseScreen) const {
    const double pickRadiusPx = 10.0;
    const double pick2 = pickRadiusPx * pickRadiusPx;

    int best = -1;
    double bestd2 = std::numeric_limits<double>::infinity();

    const auto& pts = model_.points();
    for (int i=0; i<(int)pts.size(); ++i) {
        QPointF s((double)pts[i].x, (double)pts[i].y);
        s = worldToScreen(s);
        const double dx = s.x() - mouseScreen.x();
        const double dy = s.y() - mouseScreen.y();
        const double d2 = dx*dx + dy*dy;
        if (d2 <= pick2 && d2 < bestd2) { bestd2 = d2; best = i; }
    }
    return best;
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    const QPointF m = mousePointF(event);
    const QPointF w = screenToWorld(m);

    if (live_) {
        
        dragIndex_ = pickPointIndex(m);
        if (dragIndex_ != -1) {
            dragging_ = true;
            return;
        }
    }
    
    model_.addPoint({ (long double)w.x(), (long double)w.y() });

    if (live_) {
        model_.computeHull();
        emit hullStatus((int)model_.size(), (int)model_.hullIndices().size());
    }
    update();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!live_ || !dragging_) return;
    const QPointF m = mousePointF(event);
    const QPointF w = screenToWorld(m);
    if (dragIndex_ >= 0) {
        model_.setPoint(dragIndex_, { (long double)w.x(), (long double)w.y() });
        model_.computeHull();
        emit hullStatus((int)model_.size(), (int)model_.hullIndices().size());
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
        zoomAtCenter(1.2);
        event->accept(); return;
    }
    if (event->key() == Qt::Key_Minus || event->key() == Qt::Key_Underscore) {
        zoomAtCenter(1.0/1.2);
        event->accept(); return;
    }
    if (event->key() == Qt::Key_0) {
        resetView();
        event->accept(); return;
    }
    QWidget::keyPressEvent(event);
}

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::white);

    p.save();
    p.scale(viewScale_, viewScale_);
    p.translate(viewOffset_ / viewScale_);

    const double lineW   = 2.5 / viewScale_;
    const double pntW    = 5.0 / viewScale_;
    const double radMark = 5.5 / viewScale_;

    
    const bool drawHull = live_ ? model_.hasHull() : showComputedOnce_;
    if (drawHull) {
        p.setPen(QPen(QColor(0,150,0), 3.0 / viewScale_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto& ids = model_.hullIndices();
        const auto& pts = model_.points();
        for (int i=0; i<(int)ids.size(); ++i) {
            const LDPoint &A = pts[ ids[i] ];
            const LDPoint &B = pts[ ids[(i+1)%ids.size()] ];
            p.drawLine(QPointF((double)A.x, (double)A.y),
                       QPointF((double)B.x, (double)B.y));
        }
        
        p.setPen(QPen(QColor(0,120,0), pntW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        for (int id : ids) {
            QPointF v((double)pts[id].x, (double)pts[id].y);
            p.drawEllipse(v, radMark, radMark);
        }
    }

    
    p.setPen(QPen(Qt::black, lineW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    const auto& pts = model_.points();
    for (int i=0; i<(int)pts.size(); ++i) {
        QPointF v((double)pts[i].x, (double)pts[i].y);
        p.drawEllipse(v, radMark, radMark);
    }

    
    p.setPen(Qt::darkGray);
    p.setFont(QFont("Arial", 9.0 / viewScale_));
    for (int i=0; i<(int)pts.size(); ++i) {
        QPointF v((double)pts[i].x, (double)pts[i].y);
        p.drawText(v + QPointF(7.0 / viewScale_, -7.0 / viewScale_), QString::number(i));
    }

    p.restore();

    
    p.setPen(Qt::blue);
    p.setFont(QFont("Arial", 16, QFont::Bold));
    p.drawText(10, 24, live_ ? "Онлайн: тяните точки — оболочка обновляется"
                             : "Режим по кнопке: добавьте точки и нажмите «Построить оболочку»");

    
    p.setPen(Qt::black);
    p.setFont(QFont("Consolas", 12));
    QString info = QString("points: %1   hull verts: %2")
                       .arg((int)model_.size())
                       .arg((int)model_.hullIndices().size());
    p.drawText(10, height() - 12, info);
}
