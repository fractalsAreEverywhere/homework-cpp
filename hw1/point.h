#pragma once

// Точка на плоскости.
struct Point {
    double x;
    double y;
};

// Квадрат расстояния от точки до начала координат (0, 0).
// Корень не извлекаем: если квадрат расстояния меньше, то и само расстояние меньше.
// Так быстрее и без лишней погрешности.
// inline нужен, потому что функция написана в заголовочном файле.
inline double distanceSquared(const Point& p) {
    return p.x * p.x + p.y * p.y;
}

// Компаратор для сортировки точек: a идёт раньше b, если a ближе к началу координат.
// Точки передаются по константной ссылке: без копирования и без права их менять.
inline bool closerToOrigin(const Point& a, const Point& b) {
    return distanceSquared(a) < distanceSquared(b);
}
