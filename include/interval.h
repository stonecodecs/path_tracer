#ifndef INTERVAL_H
#define INTERVAL_H

#include "rtweekend.h"

class Interval {
    public:
        double min, max;
        Interval() : min(+infinity), max(-infinity) {}
        Interval(double min, double max) : min(min), max(max) {}
        Interval(const Interval& a, const Interval& b) {
            min = a.min <= b.min ? a.min : b.min; // choose the smallest
            max = a.max >= b.max ? a.max : b.max; // choose the largest
        }

        double size() const {
            return max - min;
        }
        
        bool contains(double x) const {
            return min <= x && x <= max;
        }

        bool surrounds(double x) const {
            return min < x && x < max;
        }

        double clamp(double x) const {
            if (x < min) { return min; }
            if (x > max) { return max; }
            return x;
        }

        Interval expand(double delta) const {
            auto padding = delta / 2;
            return Interval(min - padding, max + padding);
        }

        static const Interval empty, universe;
};

const Interval Interval::empty    = Interval(+infinity, -infinity);
const Interval Interval::universe = Interval(-infinity, +infinity);


#endif 