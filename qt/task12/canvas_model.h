#pragma once

#include <QPointF>
#include <task12/point_locator.hpp>

#include <vector>

enum class Task12Phase { EditingContour, Idle, Ready, Query };

struct Task12Contour {
    bool hole = false;
    bool closed = false;
    std::vector<QPointF> pts;
};

class Task12Model {
public:
    void reset();

    bool startContour(bool hole);
    bool addPoint(const QPointF& p);
    bool closeContour();
    bool finalizeShape();

    bool editingActive() const { return activeContour_ >= 0; }
    Task12Phase phase() const { return phase_; }
    const std::vector<Task12Contour>& contours() const { return contours_; }

    bool setQueryPoint(const QPointF& p);
    bool moveQueryPoint(const QPointF& p);
    bool hasQueryPoint() const { return hasQuery_; }
    const QPointF& queryPoint() const { return queryPoint_; }
    const task12::Classification& classification() const { return lastClass_; }

private:
    std::vector<Task12Contour> contours_;
    int activeContour_ = -1;
    Task12Phase phase_ = Task12Phase::EditingContour;

    bool hasQuery_ = false;
    QPointF queryPoint_;
    task12::Classification lastClass_;

    task12::Polygon toPolygon() const;
    void updateClassification();
};

