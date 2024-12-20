#ifndef QUAD_H
#define QUAD_H

#include "hittable.h"

// Technically, creates parallelograms and not general quads
class Quad : public Hittable {
  public:
    Quad(const point4& Q, const vec4& u, const vec4& v, shared_ptr<Material> mat)
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
        if(!unit_interval.contains(a) || !unit_interval.contains(b)) {
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

inline shared_ptr<Hittable> box(const point4& a, const point4& b, shared_ptr<Material> mat)
{
    // Returns the 3D box (six sides) that contains the two opposite vertices a & b.
    auto sides = make_shared<HittableList>();

    // Construct the two opposite vertices with the minimum and maximum coordinates.
    auto min = point4(std::fmin(a.x(),b.x()), std::fmin(a.y(),b.y()), std::fmin(a.z(),b.z()));
    auto max = point4(std::fmax(a.x(),b.x()), std::fmax(a.y(),b.y()), std::fmax(a.z(),b.z()));

    auto dx = vec4(max.x() - min.x(), 0, 0);
    auto dy = vec4(0, max.y() - min.y(), 0);
    auto dz = vec4(0, 0, max.z() - min.z());

    sides->add(make_shared<Quad>(point4(min.x(), min.y(), max.z()),  dx,  dy, mat)); // front
    sides->add(make_shared<Quad>(point4(max.x(), min.y(), max.z()), -dz,  dy, mat)); // right
    sides->add(make_shared<Quad>(point4(max.x(), min.y(), min.z()), -dx,  dy, mat)); // back
    sides->add(make_shared<Quad>(point4(min.x(), min.y(), min.z()),  dz,  dy, mat)); // left
    sides->add(make_shared<Quad>(point4(min.x(), max.y(), max.z()),  dx, -dz, mat)); // top
    sides->add(make_shared<Quad>(point4(min.x(), min.y(), min.z()),  dx,  dz, mat)); // bottom

    return sides;
}

#endif