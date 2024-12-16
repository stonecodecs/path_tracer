#ifndef VEC4_H
#define VEC4_H

#include "rtweekend.h"

const double EPSILON = 1e-160;

class vec4 {
  public:
    double e[4];

    vec4() : e{0,0,0,0} {}
    vec4(double e0, double e1, double e2) : e{e0, e1, e2, 0.0} {} // vec3
    vec4(double e0, double e1, double e2, double e3) : e{e0, e1, e2, e3} {}

    double x() const { return e[0]; }
    double y() const { return e[1]; }
    double z() const { return e[2]; }
    double w() const { return e[3]; }

    vec4 operator-() const { return vec4(-e[0], -e[1], -e[2], -e[3]); }
    double operator[](int i) const { return e[i]; }
    double& operator[](int i) { return e[i]; }

    vec4& operator+=(const vec4& v) {
        e[0] += v.e[0];
        e[1] += v.e[1];
        e[2] += v.e[2];
        // e[3] += v.e[3];
        return *this;
    }

    vec4& operator*=(double t) {
        e[0] *= t;
        e[1] *= t;
        e[2] *= t;
        e[3] *= t;
        return *this;
    }

    vec4& operator/=(double t) {
        return *this *= 1/t;
    }

    double norm() const {
        return std::sqrt(norm2());
    }

    double norm2() const { // squared norm
        return e[0]*e[0] + e[1]*e[1] + e[2]*e[2];
    }

    bool near_zero() const {
        double s = 1e-8;
        return (std::fabs(e[0] < s) && std::fabs(e[1] < s) && std::fabs(e[2] < s));
    }

    static vec4 random() {
        return vec4(
            gen_random_double(),
            gen_random_double(),
            gen_random_double(),
            0.0);
    }

    static vec4 random(double min, double max) {
        return vec4(
            gen_random_double(min, max),
            gen_random_double(min, max),
            gen_random_double(min, max),
            0.0);
    }

};

// point3 is just an alias for vec4, but useful for geometric clarity in the code.
using point4 = vec4; // w component is locked to 1 here


// Vector Utility Functions

inline std::ostream& operator<<(std::ostream& out, const vec4& v) {
    return out << v.e[0] << ' ' << v.e[1] << ' ' << v.e[2] << ' ' << v.e[3];
}

inline vec4 operator+(const vec4& u, const vec4& v) {
    return vec4(
        u.e[0] + v.e[0],
        u.e[1] + v.e[1],
        u.e[2] + v.e[2],
        u.e[3]
    );
}

inline vec4 operator-(const vec4& u, const vec4& v) {
    return vec4(
        u.e[0] - v.e[0],
        u.e[1] - v.e[1],
        u.e[2] - v.e[2],
        u.e[3]
    );
}

inline vec4 operator*(const vec4& u, const vec4& v) {
    return vec4(
        u.e[0] * v.e[0],
        u.e[1] * v.e[1],
        u.e[2] * v.e[2],
        u.e[3] * v.e[3]
    );
}

inline vec4 operator*(double t, const vec4& v) {
    return vec4(
        t*v.e[0],
        t*v.e[1],
        t*v.e[2],
        t*v.e[3]
    );
}

inline vec4 operator*(const vec4& v, double t) {
    return t * v;
}

inline vec4 operator/(const vec4& v, double t) {
    return (1/t) * v;
}

inline double dot(const vec4& u, const vec4& v) {
    return u.e[0] * v.e[0]
         + u.e[1] * v.e[1]
         + u.e[2] * v.e[2];
         // + u.e[3] * v.e[3];
}

inline vec4 cross(const vec4& u, const vec4& v) {
    return vec4(u.e[1] * v.e[2] - u.e[2] * v.e[1],
                u.e[2] * v.e[0] - u.e[0] * v.e[2],
                u.e[0] * v.e[1] - u.e[1] * v.e[0],
                0.0
    );
}

inline vec4 unit_vector(const vec4& v) {
    return v / v.norm();
}

inline vec4 random_in_unit_disk() {
    while (true) {
        auto p = vec4(gen_random_double(-1, 1), gen_random_double(-1, 1), 0);
        if (p.norm2() < 1) {
            return p;
        }
    }
}

inline vec4 random_unit_vector() {
    while (true) { // keep sampling until valid gen'd vector
        vec4 p = vec4::random(-1, 1);
        auto pnorm2 = p.norm2();
        if (EPSILON < pnorm2 && pnorm2 <= 1) {
            // if inside unit sphere
            // AND preventing underflow near center of sphere
            return p / sqrt(pnorm2); // normalize
        }
    }
    // If we return a unit vector, why does it need to be within the unit sphere?
    // A: Here, we wanted a uniform distribution of scattering,
    // which would be any vector from the sphere center.
    // However, since some parts of the cube surronding it are 
    // just touching the sphere and some have lots of space (corners)
    // the distribution would no longer be uniform.

}

inline vec4 random_on_hemisphere(const vec4& normal) {
    vec4 randvec = random_unit_vector();
    return (dot(randvec, normal) > 0.0) ? randvec : -randvec;
}

inline vec4 reflect(const vec4& v, const vec4& n) {
    // note that angle 'v' that emits onto a surface
    // its reflected ray in the specular case is 
    // simply v + positive vertical component w.r.t normal
    return unit_vector(v - 2 * dot(v, n) * n);
}

inline vec4 refract(const vec4& uv, const vec4& n, double index_ratio) {
    auto unit_uv = unit_vector(uv);
    auto unit_n  = unit_vector(n);
    auto cos_theta = std::fmin(dot(-unit_uv, unit_n), 1.0);
    vec4 r_perp = index_ratio * (unit_uv + cos_theta * unit_n); // perp component
    vec4 r_vert = -std::sqrt(std::fabs(1.0 - r_perp.norm2())) * unit_n; // 'vert' comp
    return unit_vector(r_perp + r_vert);
}

inline void print_vector(const vec4& v) {
    std::cout << "vec4[" << v.x() << ", " << v.y() << ", " << v.z() << ", " << v.w() << "] ";
}

#endif