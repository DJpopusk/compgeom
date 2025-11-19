#pragma once

#include <QWidget>

#include "canvas_model.h"

class PolyBoolCanvas : public QWidget {
    Q_OBJECT
public:
    explicit PolyBoolCanvas(PolyBoolModel& model, QWidget* parent = nullptr);

    void setLive(bool on);

signals:
    void canvasChanged();

protected:
    void paintEvent(QPaintEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    QPointF screenToWorld(const QPointF& screen) const;
    QPointF worldToScreen(const QPointF& world) const;
    void zoomAtCenter(double factor);
    void resetView();
    double clamp(double v, double lo, double hi) const;
    double pickRadiusWorld() const;
    void notifyChange();

    PolyBoolModel& model_;
    bool live_ = true;
    bool dragging_ = false;
    VertexRef dragRef_;

    double viewScale_ = 1.0;
    QPointF viewOffset_{0.0, 0.0};
};

