#pragma once
#include <cmath>
#include <optional>

namespace task2 {

struct Vec2 {
    double x = 0.0;
    double y = 0.0;
};

inline Vec2 make_vec(double x, double y) { return Vec2{x, y}; }
inline Vec2 operator+(const Vec2& lhs, const Vec2& rhs) {
    return Vec2{lhs.x + rhs.x, lhs.y + rhs.y};
}
inline Vec2 operator-(const Vec2& lhs, const Vec2& rhs) {
    return Vec2{lhs.x - rhs.x, lhs.y - rhs.y};
}
inline Vec2 operator*(const Vec2& lhs, double scalar) {
    return Vec2{lhs.x * scalar, lhs.y * scalar};
}
inline Vec2 operator*(double scalar, const Vec2& rhs) {
    return Vec2{rhs.x * scalar, rhs.y * scalar};
}

inline double dot(const Vec2& a, const Vec2& b) {
    return a.x * b.x + a.y * b.y;
}
inline double cross(const Vec2& a, const Vec2& b) {
    return a.x * b.y - a.y * b.x;
}
inline double length(const Vec2& v) {
    return std::hypot(v.x, v.y);
}

bool segment_intersection(const Vec2& a, const Vec2& b,
                          const Vec2& c, const Vec2& d,
                          double eps, Vec2* out);

std::optional<Vec2> segment_intersection(const Vec2& a, const Vec2& b,
                                         const Vec2& c, const Vec2& d,
                                         double eps);

} 
