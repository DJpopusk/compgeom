#include "canvas_widget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QFont>
#include <QtMath>

static const char* kTitle =
    "Segment vs Point: 1=LEFT, -1=RIGHT, 0=ON (ε) | Zoom: +/-, Reset: 0 | Epsilon: [ / ] / E";

CanvasWidget::CanvasWidget(CanvasModel& model, QWidget *parent)
    : QWidget(parent), model_(model)
{
    setMouseTracking(true);
    setWindowTitle(kTitle);
}

double CanvasWidget::clamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
QPointF CanvasWidget::mousePointF(const QMouseEvent *e) { return e->position(); }
#else
QPointF CanvasWidget::mousePointF(const QMouseEvent *e) { return e->pos(); }
#endif

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    const QPointF m = mousePointF(event);

    if (event->button() == Qt::LeftButton) {
        QPointF w = screenToWorld(m);
        if (!model_.hasA())               { model_.setA(w); }
        else if (!model_.hasB())          { model_.setB(w); }
        else                              { model_.setA(w); model_.clearB(); }
        update();

    } else if (event->button() == Qt::RightButton) {
        model_.setP(screenToWorld(m));
        update();
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

    if (event->key() == Qt::Key_BracketRight) {        // ]
        model_.setEps(model_.eps() + 0.5);
        update(); event->accept(); return;
    }
    if (event->key() == Qt::Key_BracketLeft) {         // [
        model_.setEps(model_.eps() - 0.5);
        update(); event->accept(); return;
    }
    if (event->key() == Qt::Key_E) {                   // E
        model_.setEps(1.0);
        update(); event->accept(); return;
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

    const double lineW   = 4.0 / viewScale_;
    const double pntW    = 5.0 / viewScale_;
    const double radMark = 6.0 / viewScale_;

    p.setPen(QPen(Qt::black, lineW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (model_.hasA()) p.drawEllipse(model_.A(), radMark, radMark);
    if (model_.hasA() && model_.hasB()) {
        p.drawLine(model_.A(), model_.B());
        p.drawEllipse(model_.B(), radMark, radMark);
    }

    if (model_.hasP()) {
        p.setPen(QPen(Qt::red, pntW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        p.drawPoint(model_.P());
    }

    p.setPen(Qt::darkGray);
    p.setFont(QFont("Arial", 10.0 / viewScale_));
    if (model_.hasA()) p.drawText(model_.A() + QPointF(8.0 / viewScale_, -8.0 / viewScale_),
                   QString("A( %1, %2 )").arg(model_.A().x(),0,'f',0).arg(model_.A().y(),0,'f',0));
    if (model_.hasB()) p.drawText(model_.B() + QPointF(8.0 / viewScale_, -8.0 / viewScale_),
                   QString("B( %1, %2 )").arg(model_.B().x(),0,'f',0).arg(model_.B().y(),0,'f',0));
    if (model_.hasP()) p.drawText(model_.P() + QPointF(8.0 / viewScale_, -8.0 / viewScale_),
                   QString("P( %1, %2 )").arg(model_.P().x(),0,'f',0).arg(model_.P().y(),0,'f',0));

    p.restore();

    p.setPen(Qt::blue);
    p.setFont(QFont("Arial", 22, QFont::Bold));
    const int r = model_.relation();
    QString res = (r == 1 ? "1" : r == -1 ? "-1" : r == 0 ? "0" :
                                  "ЛКМ: A→B, ПКМ: P, Zoom: +/-, 0, ε: [ / ] / E");
    p.drawText(10, 32, res);

    p.setPen(Qt::black);
    p.setFont(QFont("Consolas", 12));
    QString coords;
    if (model_.hasA()) coords += QString("A = (%1, %2)\n").arg(model_.A().x(),0,'f',1).arg(model_.A().y(),0,'f',1);
    else               coords += "A = (—, —)\n";
    if (model_.hasB()) coords += QString("B = (%1, %2)\n").arg(model_.B().x(),0,'f',1).arg(model_.B().y(),0,'f',1);
    else               coords += "B = (—, —)\n";
    if (model_.hasP()) coords += QString("P = (%1, %2)\n").arg(model_.P().x(),0,'f',1).arg(model_.P().y(),0,'f',1);
    else               coords += "P = (—, —)\n";
    coords += QString("ε (tolerance) = %1").arg(model_.eps(), 0, 'f', 2);

    const int margin = 10;
    QRect box(margin, height() - margin - 4*18, 320, 4*18);
    p.drawText(box, Qt::AlignLeft | Qt::AlignVCenter, coords);
}

QPointF CanvasWidget::screenToWorld(const QPointF &screen) const {
    // screen = scale * world + offset  =>  world = (screen - offset) / scale
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
