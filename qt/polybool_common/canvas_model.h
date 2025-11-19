#pragma once

#include <QPointF>

#include <task10/polygon_boolean.hpp>

#include <vector>

struct PolyBoolConfig {
    bool allowHoles = false;
    bool allowExtraContours = false;
};

enum class PolyBoolPhase {
    EditingA,
    IdleA,
    EditingB,
    IdleB,
    Ready
};

enum class PolyBoolMode { Intersection, Union, Difference };

struct VertexRef {
    int poly = -1;
    int contour = -1;
    int index = -1;
    bool valid() const { return poly >= 0 && contour >= 0 && index >= 0; }
};

struct ResultLoop {
    bool hole = false;
    std::vector<QPointF> points;
};

class PolyBoolModel {
public:
    PolyBoolModel(const PolyBoolConfig& cfgA,
                  const PolyBoolConfig& cfgB);

    void reset();

    PolyBoolPhase phase() const { return phase_; }

    PolyBoolMode mode() const { return mode_; }
    void setMode(PolyBoolMode mode);

    bool startContour(bool hole);
    bool closeContour();
    bool startSecondPolygon();
    bool finishSecondPolygon();

    bool addPoint(const QPointF& p);
    bool deletePoint(const QPointF& p, double radius);
    bool beginDrag(const QPointF& p, double radius, VertexRef& ref) const;
    bool moveVertex(const VertexRef& ref, const QPointF& p);

    const std::vector<ResultLoop>& result() const { return result_; }

    struct Outline {
        std::vector<QPointF> pts;
        bool closed = false;
        bool hole = false;
    };

    const std::vector<Outline>& pointsA() const { return outlinesA_; }
    const std::vector<Outline>& pointsB() const { return outlinesB_; }
    const VertexRef& activeHandle() const { return activeHandle_; }

    bool hasClosedA() const;
    bool hasClosedB() const;

private:
    struct Contour {
        bool hole = false;
        bool closed = false;
        std::vector<QPointF> pts;
    };

    struct PolyStore {
        std::vector<Contour> contours;
        int active = -1;
    };

    const PolyBoolConfig cfg_[2];
    PolyStore polys_[2];
    PolyBoolPhase phase_ = PolyBoolPhase::EditingA;
    PolyBoolMode mode_ = PolyBoolMode::Intersection;
    mutable VertexRef activeHandle_;

    std::vector<ResultLoop> result_;
    std::vector<Outline> outlinesA_;
    std::vector<Outline> outlinesB_;

    PolyStore& currentPoly();
    const PolyStore& currentPoly() const;

    void syncOutlines();
    void rebuildResult();
    task10::Polygon makePolygon(const PolyStore& poly) const;
    static task10::Loop makeLoop(const Contour& contour);
    bool addPointTo(PolyStore& store, const QPointF& p);
    bool canAddHole(int index) const;
};
