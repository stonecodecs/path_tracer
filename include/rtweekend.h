#ifndef RTWEEKEND_H
#define RTWEEKEND_H

#include <cmath>
#include <iostream>
#include <limits>
#include <memory>
#include <vector>
#include <cstdlib>
// stores common constants

// C++ Std Usings

using std::make_shared;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

enum Axis { // 0, 1, 2
    X, Y, Z
};

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

inline double gen_random_double() { // this is uniform
    // returns random real # in [0, 1)
    return std::rand() / (RAND_MAX + 1.0);
}


inline double gen_random_double(double min, double max) {
    // returns random real # in [min, max)
    return min + (max - min) * gen_random_double();
}

inline int gen_random_int(int min, int max) {
    return int(gen_random_double(min, max+1));
}
// Common Headers

#include "color.h"
#include "ray.h"
#include "vec4.h"
#include "interval.h"

#endif