#include "task10/polygon_boolean.hpp"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

#include "clipper2/clipper.h"

namespace task10 {
namespace {

constexpr double kScale = 1e6;

Clipper2Lib::Path64 to_path(const Loop& loop) {
    Clipper2Lib::Path64 path;
    path.reserve(loop.vertices.size());
    for (const auto& v : loop.vertices) {
        const double xs = std::clamp(v.x * kScale,
                                     static_cast<double>(std::numeric_limits<int64_t>::min() / 2),
                                     static_cast<double>(std::numeric_limits<int64_t>::max() / 2));
        const double ys = std::clamp(v.y * kScale,
                                     static_cast<double>(std::numeric_limits<int64_t>::min() / 2),
                                     static_cast<double>(std::numeric_limits<int64_t>::max() / 2));
        path.emplace_back(static_cast<int64_t>(std::llround(xs)),
                          static_cast<int64_t>(std::llround(ys)));
    }
    return path;
}

Clipper2Lib::Paths64 to_paths(const Polygon& poly) {
    Clipper2Lib::Paths64 paths;
    for (const auto& loop : poly.loops) {
        if (loop.vertices.size() < 3) continue;
        paths.push_back(to_path(loop));
    }
    return paths;
}

Loop from_path(const Clipper2Lib::Path64& path, bool hole) {
    Loop loop;
    loop.hole = hole;
    loop.vertices.reserve(path.size());
    for (const auto& pt : path) {
        loop.vertices.push_back(Point{static_cast<double>(pt.x) / kScale,
                                      static_cast<double>(pt.y) / kScale});
    }
    return loop;
}

void append_children(const Clipper2Lib::PolyPath64& node,
                     std::vector<Polygon>& result,
                     Polygon& owner) {
    for (size_t i = 0; i < node.Count(); ++i) {
        const auto& child = *node.Child(i);
        if (child.IsHole()) {
            owner.loops.push_back(from_path(child.Polygon(), true));
            append_children(child, result, owner);
        } else {
            result.emplace_back();
            auto& next = result.back();
            next.loops.push_back(from_path(child.Polygon(), false));
            append_children(child, result, next);
        }
    }
}

std::vector<Polygon> from_tree(const Clipper2Lib::PolyTree64& tree) {
    std::vector<Polygon> polys;
    for (size_t i = 0; i < tree.Count(); ++i) {
        const auto& node = *tree[i];
        if (node.IsHole()) continue;
        polys.emplace_back();
        auto& poly = polys.back();
        poly.loops.push_back(from_path(node.Polygon(), false));
        append_children(node, polys, poly);
    }
    return polys;
}

Clipper2Lib::ClipType clip_type(Operation op) {
    switch (op) {
        case Operation::Intersection:
            return Clipper2Lib::ClipType::Intersection;
        case Operation::Union:
            return Clipper2Lib::ClipType::Union;
        case Operation::DifferenceAB:
            return Clipper2Lib::ClipType::Difference;
    }
    return Clipper2Lib::ClipType::Union;
}

}  

std::vector<Polygon> boolean_operation(const Polygon& a,
                                       const Polygon& b,
                                       Operation op) {
    Clipper2Lib::Clipper64 clipper;
    clipper.AddSubject(to_paths(a));
    clipper.AddClip(to_paths(b));
    Clipper2Lib::PolyTree64 tree;
    Clipper2Lib::Paths64 open;
    clipper.Execute(clip_type(op),
                    Clipper2Lib::FillRule::NonZero,
                    tree,
                    open);
    return from_tree(tree);
}

}
