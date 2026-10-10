#pragma once

// Точка на плоскости.
struct Point {
    double x;
    double y;
};

// Квадрат расстояния от точки до начала координат.
inline double distanceSquared(const Point& p) {
    return p.x * p.x + p.y * p.y;
}

// Компаратор: a раньше b, если a ближе к началу координат.
inline bool closerToOrigin(const Point& a, const Point& b) {
    return distanceSquared(a) < distanceSquared(b);
}
