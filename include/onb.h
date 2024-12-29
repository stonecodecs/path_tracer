#ifndef ONB_H
#define ONB_H

#include "rtweekend.h"

class ONB {
    public:
        ONB(const vec4& n) {
            axis[2] = unit_vector(n); // normal is the z-axis
            vec4 a  = (std::fabs(axis[2].x()) > 0.9) ? vec4(0,1,0) : vec4(1,0,0);
            axis[1] = unit_vector(cross(axis[2], a));
            axis[0] = cross(axis[1], axis[2]);
        }

        const vec4& u() const { return axis[0]; }
        const vec4& v() const { return axis[1]; }
        const vec4& w() const { return axis[2]; }

        vec4 transform(const vec4& v) const {
            // v is vector from the ONB
            // v (wrt ONB) to v (wrt world coordinates)
            return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
        } 
    private:
        vec4 axis[3];
};

#endif