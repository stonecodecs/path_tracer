#ifndef AABB_H
#define AABB_H

#include "rtweekend.h"
#include <algorithm>

int argmax(const std::vector<double>& vec) {
    return std::distance(vec.begin(), std::max_element(vec.begin(), vec.end()));
}

class AABB {
    public:
        Interval x, y, z; // these are all (+inf, -inf) from interval.h
        AABB() {}
        AABB(const Interval& x, const Interval& y, const Interval& z) :
        x(x), y(y), z(z) {}
        AABB(const point4& a, const point4& b) {
            // 2 corners of the bounding box that fully define the interval
            x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
            y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
            z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);
        }
        AABB(const AABB& boxA, const AABB& boxB) {
            x = Interval(boxA.x, boxB.x);
            y = Interval(boxA.y, boxB.y);
            z = Interval(boxA.z, boxB.z);
        }

        const Interval& axis_interval(int n) const {
            if (n == 1) return y;
            if (n == 2) return z;
            return x;
        }

        bool hit(const Ray& r, Interval ray_t) const {
            const point4& ray_o = r.o();
            const point4& ray_d = r.d();

            for (int axis = 0; axis < 3; axis++) {
                const Interval& ax = axis_interval(axis);
                const double adinv = 1.0 / ray_d[axis];
                // t_ = min/max(x_ - Q_) / adinv_ 
                auto t0 = (ax.min - ray_o[axis]) * adinv;
                auto t1 = (ax.max - ray_o[axis]) * adinv;

                // t0 entering interval, less than t1
                if (t0 < t1) {
                    if (t0 > ray_t.min) ray_t.min = t0;
                    if (t1 < ray_t.max) ray_t.max = t1;
                } else { // negative ray direction
                    if (t1 > ray_t.min) ray_t.min = t1;
                    if (t0 < ray_t.max) ray_t.max = t0;
                }
                
                // no overlap, exit early
                if (ray_t.max <= ray_t.min) {
                    return false;
                }
            }
            return true; // all axes have overlap
        }

        int longest_axis() const {
            return argmax({x.size(), y.size(), z.size()});
        }

        static const AABB empty, universe;
};

const AABB AABB::empty = AABB(Interval::empty, Interval::empty, Interval::empty);
const AABB AABB::universe = AABB(Interval::universe, Interval::universe, Interval::universe);

#endif