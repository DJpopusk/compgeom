#include "canvas_widget.h"
#include <QMouseEvent>
#include <QKeyEvent>
#include <QPainter>
#include <QtMath>

CanvasWidget::CanvasWidget(CanvasModel& model, QWidget *parent)
    : QWidget(parent), model_(model)
{
    setMouseTracking(true);
}

QPointF CanvasWidget::mousePointF(const QMouseEvent *e) {
#if QT_VERSION >= QT_VERSION_CHECK(6,0,0)
    return e->position();
#else
    return e->pos();
#endif
}
double CanvasWidget::dclamp(double v, double lo, double hi) {
    return v < lo ? lo : (v > hi ? hi : v);
}

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

void CanvasWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() != Qt::LeftButton) return;
    const QPointF m = mousePointF(event);
    const QPointF w = screenToWorld(m);

    
    LDPoint W{ (long double)w.x(), (long double)w.y() };

    if (clickStage_ == 0) {
        model_.setA(W);
        clickStage_ = 1;
    } else if (clickStage_ == 1) {
        model_.setB(W);
        clickStage_ = 2;
    } else {
        model_.setP(W);
        clickStage_ = 0;
    }
    emit relationChanged(model_.relation());
    update();
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

    const double lineW   = 4.0 / viewScale_;
    const double pntW    = 5.0 / viewScale_;
    const double radMark = 6.0 / viewScale_;

    
    p.setPen(QPen(Qt::black, lineW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
    if (model_.hasA()) {
        QPointF a((double)model_.A().x, (double)model_.A().y);
        p.drawEllipse(a, radMark, radMark);
        if (model_.hasB()) {
            QPointF b((double)model_.B().x, (double)model_.B().y);
            p.drawLine(a, b);
            p.drawEllipse(b, radMark, radMark);
        }
    }
    if (model_.hasP()) {
        p.setPen(QPen(Qt::red, pntW, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        QPointF pp((double)model_.P().x, (double)model_.P().y);
        p.drawPoint(pp);
    }

    
    p.setPen(Qt::darkGray);
    p.setFont(QFont("Arial", 10.0 / viewScale_));
    auto label = [&](const LDPoint& pt, const char* name){
        QPointF d((double)pt.x, (double)pt.y);
        p.drawText(d + QPointF(8.0 / viewScale_, -8.0 / viewScale_),
                   QString("%1( %2, %3 )").arg(name).arg((double)pt.x,0,'f',0).arg((double)pt.y,0,'f',0));
    };
    if (model_.hasA()) label(model_.A(), "A");
    if (model_.hasB()) label(model_.B(), "B");
    if (model_.hasP()) label(model_.P(), "P");

    p.restore();

    
    p.setPen(Qt::blue);
    p.setFont(QFont("Arial", 22, QFont::Bold));
    const int r = model_.relation();
    QString res = (r == 1 ? "1" : r == -1 ? "-1" : r == 0 ? "0" : "—");
    p.drawText(10, 32, res);

    
    p.setPen(Qt::black);
    p.setFont(QFont("Consolas", 12));
    QString coords;
    if (model_.hasA()) coords += QString("A = (%1, %2)\n").arg((double)model_.A().x,0,'f',6).arg((double)model_.A().y,0,'f',6);
    else               coords += "A = (—, —)\n";
    if (model_.hasB()) coords += QString("B = (%1, %2)\n").arg((double)model_.B().x,0,'f',6).arg((double)model_.B().y,0,'f',6);
    else               coords += "B = (—, —)\n";
    if (model_.hasP()) coords += QString("P = (%1, %2)\n").arg((double)model_.P().x,0,'f',6).arg((double)model_.P().y,0,'f',6);
    else               coords += "P = (—, —)\n";
    coords += QString("eps = %1").arg((double)model_.eps(),0,'g',10);

    const int margin = 10;
    QRect box(margin, height() - margin - 5*18, 360, 5*18);
    p.drawText(box, Qt::AlignLeft | Qt::AlignVCenter, coords);
}
