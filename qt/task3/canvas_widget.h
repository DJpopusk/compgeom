#pragma once
#include <QWidget>
#include "canvas_model.h"

class QMouseEvent;
class QKeyEvent;

class CanvasWidget : public QWidget {
    Q_OBJECT
public:
    explicit CanvasWidget(CanvasModel& model, QWidget *parent = nullptr);

    
    void resetView();

signals:
    void relationChanged(int r);

protected:
    void mousePressEvent(QMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;
    void paintEvent(QPaintEvent *) override;

private:
    static QPointF mousePointF(const QMouseEvent *e);
    static double  dclamp(double v, double lo, double hi);
    QPointF screenToWorld(const QPointF &screen) const;
    QPointF worldToScreen(const QPointF &world) const;
    void zoomAtCenter(double factor);

private:
    CanvasModel& model_;
    double  viewScale_  = 1.0;
    QPointF viewOffset_ = QPointF(0, 0);
    int     clickStage_ = 0; 
};
