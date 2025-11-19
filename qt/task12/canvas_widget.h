#pragma once

#include <QWidget>

#include "canvas_model.h"

class Task12Canvas : public QWidget {
    Q_OBJECT
public:
    explicit Task12Canvas(Task12Model& model, QWidget* parent = nullptr);

    void setLive(bool on);

signals:
    void modelChanged();

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
    void zoomAt(double factor);
    void resetView();
    double clamp(double v, double lo, double hi) const;
    double pickRadius() const;
    QColor queryColor(task12::Region region) const;

    Task12Model& model_;
    bool live_ = true;
    bool dragging_ = false;

    double viewScale_ = 1.0;
    QPointF viewOffset_{0.0, 0.0};
};

