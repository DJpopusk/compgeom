#pragma once

#include <QPointF>
#include <task11/point_locator.hpp>

#include <vector>

enum class Task11Phase { Collecting, HullReady, Query };

class Task11Model {
public:
    void reset();

    bool addPoint(const QPointF& p);
    bool finishHull();

    bool hasHull() const { return phase_ != Task11Phase::Collecting && !hull_.empty(); }
    const std::vector<QPointF>& rawPoints() const { return points_; }
    const std::vector<QPointF>& hull() const { return hull_; }
    Task11Phase phase() const { return phase_; }

    bool setQueryPoint(const QPointF& p);
    bool moveQueryPoint(const QPointF& p);
    bool hasQueryPoint() const { return hasQuery_; }
    const QPointF& queryPoint() const { return queryPoint_; }
    const task11::Classification& classification() const { return lastClass_; }

private:
    std::vector<QPointF> points_;
    std::vector<QPointF> hull_;
    Task11Phase phase_ = Task11Phase::Collecting;

    bool hasQuery_ = false;
    QPointF queryPoint_;
    task11::Classification lastClass_;

    void updateClassification();
};

