#pragma once
#include <QWidget>
#include <QPointF>
#include "canvas_model.h"

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    explicit CanvasWidget(CanvasModel& model, QWidget *parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *) override;

private:
    static double clamp(double v, double lo, double hi);
    static QPointF mousePointF(const QMouseEvent *e);

    
    QPointF screenToWorld(const QPointF &screen) const;
    QPointF worldToScreen(const QPointF &world) const;
    void zoomAtCenter(double factor);
    void resetView();

private:
    CanvasModel& model_;
    double  viewScale_  = 1.0;
    QPointF viewOffset_ = QPointF(0, 0);
};
