#pragma once
#include <QWidget>
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
    void resetView();

signals:
    void hullStatus(int points, int hullVerts);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *) override;

private:
    static QPointF mousePointF(const QMouseEvent *e);
    static double  dclamp(double v, double lo, double hi);
    QPointF screenToWorld(const QPointF &screen) const;
    QPointF worldToScreen(const QPointF &world) const;
    void zoomAtCenter(double factor);
    int  pickPointIndex(const QPointF& mouseScreen) const;

private:
    CanvasModel& model_;

    double  viewScale_  = 1.0;
    QPointF viewOffset_ = QPointF(0, 0);

    bool live_ = false;
    bool dragging_ = false;
    int  dragIndex_ = -1;

    bool showComputedOnce_ = false; 
};
