#ifndef HITTABLE_H
#define HITTABLE_H

#include "rtweekend.h"
#include "aabb.h"

class Material;

class Hit {
    public:
    point4 p; // hitpoint
    vec4 normal; // normal vector from p
    shared_ptr<Material> mat;
    double t; // intersection 't'
    bool front_face;

    void set_face_normal(const Ray& r, const vec4& normal_out) {
        // normal_out assumed to be a unit vector
        front_face = dot(r.d(), normal_out) < 0;
        normal = front_face ? normal_out : -normal_out;
    }

};

class Hittable {
    public:
        // virtual functions are to be overridden by
        // subclasses of Hittable and implemented there.
        virtual ~Hittable() = default; // -- let compiler handle destructor
        virtual bool hit(
            const Ray& r,
            Interval ray_t,
            Hit& hit_record) const = 0;
        virtual AABB bounding_box() const = 0;
};

#endif