#include "canvas_widget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QFont>
#include <QtMath>
#include <limits>

CanvasWidget::CanvasWidget(CanvasModel& model, QWidget *parent)
    : QWidget(parent), model_(model)
{
    setMouseTracking(true);
}

void CanvasWidget::setLive(bool on) {
    live_ = on;
    if (live_) showComputedOnce_ = false; 
    update();
}

void CanvasWidget::computeOnce() {
    QPointF x;
    showComputedOnce_ = model_.segmentIntersection(&x);
    if (showComputedOnce_) lastIntersection_ = x;
    emit intersectionAvailable(showComputedOnce_);
    update();
}

double CanvasWidget::clamp(double v, double lo, double hi) {
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
    const double next = clamp(old * factor, 0.1, 50.0);
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

    for (int i=0;i<4;++i) {
        if (!model_.has(static_cast<CanvasModel::PointId>(i))) continue;
        const QPointF s = worldToScreen(model_.point(static_cast<CanvasModel::PointId>(i)));
        const double dx = s.x() - mouseScreen.x();
        const double dy = s.y() - mouseScreen.y();
        const double d2 = dx*dx + dy*dy;
        if (d2 <= pick2 && d2 < bestd2) { bestd2 = d2; best = i; }
    }
    return best;
}

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    const QPointF m = mousePointF(event);

    if (event->button() == Qt::LeftButton) {
        if (live_) {
            
            dragIndex_ = pickPointIndex(m);
            if (dragIndex_ != -1) {
                dragging_ = true;
            } else {
                
                model_.placeNext(screenToWorld(m));
                if (live_) showComputedOnce_ = false;
                emit intersectionAvailable(model_.ready() && model_.segmentIntersection(nullptr));
                update();
            }
        } else {
            model_.placeNext(screenToWorld(m));
            update();
        }
    }
}

void CanvasWidget::mouseMoveEvent(QMouseEvent *event) {
    if (!live_ || !dragging_) return;
    const QPointF m = mousePointF(event);
    const QPointF w = screenToWorld(m);
    if (dragIndex_ >= 0) {
        model_.setPoint(static_cast<CanvasModel::PointId>(dragIndex_), w);
        
        QPointF x;
        const bool ok = model_.segmentIntersection(&x);
        lastIntersection_ = x;
        emit intersectionAvailable(ok);
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

void CanvasWidget::resizeEvent(QResizeEvent*) { update(); }

void CanvasWidget::paintEvent(QPaintEvent *) {
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.fillRect(rect(), Qt::white);

    p.save();
    p.scale(viewScale_, viewScale_);
    p.translate(viewOffset_ / viewScale_);

    const double lineW   = 3.0 / viewScale_;
    const double pntW    = 5.0 / viewScale_;
    const double radMark = 6.0 / viewScale_;

    
    p.setPen(QPen(Qt::black, lineW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (model_.has(CanvasModel::A)) p.drawEllipse(model_.point(CanvasModel::A), radMark, radMark);
    if (model_.has(CanvasModel::B) && model_.has(CanvasModel::A)) {
        p.drawLine(model_.point(CanvasModel::A), model_.point(CanvasModel::B));
        p.drawEllipse(model_.point(CanvasModel::B), radMark, radMark);
    }
    if (model_.has(CanvasModel::C)) p.drawEllipse(model_.point(CanvasModel::C), radMark, radMark);
    if (model_.has(CanvasModel::D) && model_.has(CanvasModel::C)) {
        p.drawLine(model_.point(CanvasModel::C), model_.point(CanvasModel::D));
        p.drawEllipse(model_.point(CanvasModel::D), radMark, radMark);
    }

    
    p.setPen(Qt::darkGray);
    p.setFont(QFont("Arial", 10.0 / viewScale_));
    auto label = [&](const QPointF& pt, const char* name){
        p.drawText(pt + QPointF(8.0 / viewScale_, -8.0 / viewScale_),
                   QString("%1( %2, %3 )").arg(name).arg(pt.x(),0,'f',0).arg(pt.y(),0,'f',0));
    };
    if (model_.has(CanvasModel::A)) label(model_.point(CanvasModel::A), "A");
    if (model_.has(CanvasModel::B)) label(model_.point(CanvasModel::B), "B");
    if (model_.has(CanvasModel::C)) label(model_.point(CanvasModel::C), "C");
    if (model_.has(CanvasModel::D)) label(model_.point(CanvasModel::D), "D");

    
    bool drawX = false;
    QPointF X{};
    if (live_) {
        if (model_.segmentIntersection(&X)) drawX = true;
    } else if (showComputedOnce_) {
        X = lastIntersection_;
        drawX = true;
    }
    if (drawX) {
        p.setPen(QPen(Qt::red, pntW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawLine(X + QPointF(-8.0/viewScale_, 0), X + QPointF(8.0/viewScale_, 0));
        p.drawLine(X + QPointF(0, -8.0/viewScale_), X + QPointF(0, 8.0/viewScale_));
        p.drawText(X + QPointF(10.0 / viewScale_, -10.0 / viewScale_),
                   QString("X( %1, %2 )").arg(X.x(),0,'f',2).arg(X.y(),0,'f',2));
    }

    p.restore();

    
    p.setPen(Qt::blue);
    p.setFont(QFont("Arial", 16, QFont::Bold));
    p.drawText(10, 24, live_ ? "Онлайн режим: перетаскивайте вершины" :
                           "Режим по кнопке: поставьте 4 точки и нажмите 'Вычислить'");

    
    p.setPen(Qt::black);
    p.setFont(QFont("Consolas", 12));
    QString coords;
    auto fmt = [&](CanvasModel::PointId id, const char* n){
        if (model_.has(id)) return QString("%1 = (%2, %3)\n")
                .arg(n).arg(model_.point(id).x(),0,'f',1).arg(model_.point(id).y(),0,'f',1);
        return QString("%1 = (—, —)\n").arg(n);
    };
    coords += fmt(CanvasModel::A, "A");
    coords += fmt(CanvasModel::B, "B");
    coords += fmt(CanvasModel::C, "C");
    coords += fmt(CanvasModel::D, "D");
    coords += QString("eps = %1").arg(model_.eps(),0,'f',2);

    const int margin = 10;
    QRect box(margin, height() - margin - 5*18, 340, 5*18);
    p.drawText(box, Qt::AlignLeft | Qt::AlignVCenter, coords);
}
