#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

// setup like general quads, but different
class Triangle : public Hittable {
  public:
    Triangle(const point4& Q, const vec4& u, const vec4& v, shared_ptr<Material> mat)
      : Q(Q), u(u), v(v), mat(mat)
    {
        auto n = cross(u,v);
        normal = unit_vector(n); // normal of plane (quad)
        D = dot(normal, Q); // Q lies on plane, which is enough to solve for D
        w = n / dot(n, n); // unit normal
        set_bounding_box();
    }

    virtual void set_bounding_box() {
        // Compute the bounding box of all four vertices.
        // u and v are the defining edge vectors
        // Q is the "origin" of u and v vectors
        auto bbox_diagonal1 = AABB(Q, Q + u + v);
        auto bbox_diagonal2 = AABB(Q + u, Q + v);
        bbox = AABB(bbox_diagonal1, bbox_diagonal2);
    }

    AABB bounding_box() const override { return bbox; }

    bool hit(const Ray& r, Interval ray_t, Hit& rec) const override {
        auto denom = dot(normal, r.d());

        // ray parallel to plane case: no hit
        if (std::fabs(denom) < 1e-8) { return false; }

        // no hit when 't' is outside the ray interval
        auto t = (D - dot(normal, r.o())) / denom;
        if (!ray_t.contains(t)) { return false; }

        auto intersection = r.at(t); // hits plane
        vec4 planar_hit_vec = intersection - Q; // P - Q
        auto alpha = dot(w, cross(planar_hit_vec, v));
        auto beta  = dot(w, cross(u, planar_hit_vec));

        if (!is_interior(alpha, beta, rec)) { return false; }
        
        // fill out rec if valid intersection
        rec.t = t;
        rec.p = intersection;
        rec.mat = mat;
        rec.set_face_normal(r, normal);

        return true;
    }

    virtual bool is_interior(double a, double b, Hit& rec) const {
        // hit point is interior to primitive's region of plane
        Interval unit_interval = Interval(0, 1);
        if(!unit_interval.contains(a) || !unit_interval.contains(b) || !unit_interval.contains(a + b)) {
            return false;
        }

        rec.u = a;
        rec.v = b;
        return true;
    }

  private:
    point4 Q;
    vec4 u, v;
    vec4 w;
    shared_ptr<Material> mat;
    AABB bbox;
    vec4 normal;
    double D;
};

#endif