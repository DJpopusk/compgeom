#pragma once
#include <QWidget>
#include <QPointF>
#include "canvas_model.h"

class QMouseEvent;
class QKeyEvent;

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    explicit CanvasWidget(CanvasModel& model, QWidget *parent = nullptr);

    
    void setLive(bool on);
    bool live() const { return live_; }

    
    void computeOnce();

signals:
    void intersectionAvailable(bool ok); 

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent*) override;

private:
    static double clamp(double v, double lo, double hi);
    static QPointF mousePointF(const QMouseEvent *e);
    QPointF screenToWorld(const QPointF &screen) const;
    QPointF worldToScreen(const QPointF &world) const;
    void zoomAtCenter(double factor);
    void resetView();
    int pickPointIndex(const QPointF& mouseScreen) const; 

private:
    CanvasModel& model_;
    double  viewScale_  = 1.0;
    QPointF viewOffset_ = QPointF(0, 0);

    bool live_ = false;
    bool dragging_ = false;
    int dragIndex_ = -1;

    
    bool showComputedOnce_ = false;
    QPointF lastIntersection_{};
};
