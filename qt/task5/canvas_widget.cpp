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
    setToolTip("ЛКМ: добавить точку. Онлайн — перетаскивать точки. Zoom: +/-, Reset: 0");
}

void CanvasWidget::setLive(bool on) {
    live_ = on;
    if (live_) {
        showComputedOnce_ = false;
        model_.computeDelaunay();
        emit triStatus((int)model_.size(), (int)model_.triangles().size());
    }
    update();
}

void CanvasWidget::computeOnce() {
    showComputedOnce_ = model_.computeDelaunay();
    emit triStatus((int)model_.size(), (int)model_.triangles().size());
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
        // пробуем схватить существующую точку
        dragIndex_ = pickPointIndex(m);
        if (dragIndex_ != -1) {
            dragging_ = true;
            return;
        }
    }
    // иначе — добавляем новую точку
    model_.addPoint({ (long double)w.x(), (long double)w.y() });

    if (live_) {
        model_.computeDelaunay();
        emit triStatus((int)model_.size(), (int)model_.triangles().size());
    }
    update();
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!live_ || !dragging_) return;
    const QPointF m = mousePointF(event);
    const QPointF w = screenToWorld(m);
    if (dragIndex_ >= 0) {
        model_.setPoint(dragIndex_, { (long double)w.x(), (long double)w.y() });
        model_.computeDelaunay();
        emit triStatus((int)model_.size(), (int)model_.triangles().size());
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

    const double lineW   = 2.0 / viewScale_;
    const double pntW    = 4.5 / viewScale_;
    const double radMark = 5.0 / viewScale_;

    // 1) Триангуляция (если есть и либо онлайн-режим, либо нажимали кнопку)
    const bool drawTri = live_ ? model_.hasTriangulation() : showComputedOnce_;
    if (drawTri) {
        p.setPen(QPen(QColor(30, 60, 200), 2.0 / viewScale_, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        const auto& T = model_.triangles();
        const auto& P = model_.points();
        for (const auto& tr : T) {
            const QPointF A((double)P[tr.a].x, (double)P[tr.a].y);
            const QPointF B((double)P[tr.b].x, (double)P[tr.b].y);
            const QPointF C((double)P[tr.c].x, (double)P[tr.c].y);
            p.drawLine(A, B);
            p.drawLine(B, C);
            p.drawLine(C, A);
        }
    }

    // 2) Все точки
    p.setPen(QPen(Qt::black, lineW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    const auto& pts = model_.points();
    for (int i=0; i<(int)pts.size(); ++i) {
        QPointF v((double)pts[i].x, (double)pts[i].y);
        p.drawEllipse(v, radMark, radMark);
    }

    // 3) Индексы
    p.setPen(Qt::darkGray);
    p.setFont(QFont("Arial", 9.0 / viewScale_));
    for (int i=0; i<(int)pts.size(); ++i) {
        QPointF v((double)pts[i].x, (double)pts[i].y);
        p.drawText(v + QPointF(6.0 / viewScale_, -6.0 / viewScale_), QString::number(i));
    }

    p.restore();

    p.setPen(Qt::blue);
    p.setFont(QFont("Arial", 16, QFont::Bold));
    

    // статус
    p.setPen(Qt::black);
    p.setFont(QFont("Consolas", 12));
    QString info = QString("points: %1   triangles: %2")
                       .arg((int)model_.size())
                       .arg((int)model_.triangles().size());
    p.drawText(10, height() - 12, info);
}
