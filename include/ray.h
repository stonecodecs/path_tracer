#ifndef RAY_H
#define RAY_H

#include "vec4.h"

class Ray {
  public:
    Ray() {}

    Ray(const point4& origin, const vec4& direction, double time) : orig(origin), dir(direction), t(time) {}
    Ray(const point4& origin, const vec4& direction) : Ray(origin, direction, 0.0) {}

    const point4& o() const  { return orig; }
    const vec4& d() const { return dir; }
    double time() const { return t; }

    point4 at(double t) const {
        return orig + t*dir;
    }

  private:
    point4 orig;
    vec4 dir;
    double t; // time 
};

#endif