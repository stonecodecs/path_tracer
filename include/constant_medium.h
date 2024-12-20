#ifndef CONSTANT_MEDIUM_H
#define CONSTANT_MEDIUM_H

#include "hittable.h"
#include "material.h"
#include "texture.h"

class ConstantMedium : public Hittable {
    // constant density medium (mist/smokelike)
  public:
    ConstantMedium(shared_ptr<Hittable> boundary, double density, shared_ptr<Texture> tex)
      : boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<Isotropic>(tex))
    {}

    ConstantMedium(shared_ptr<Hittable> boundary, double density, const Color& albedo)
      : boundary(boundary), neg_inv_density(-1/density),
        phase_function(make_shared<Isotropic>(albedo))
    {}

    bool hit(const Ray& r, Interval Ray_t, Hit& rec) const override {
        Hit rec1, rec2;

        if (!boundary->hit(r, Interval::universe, rec1))
            return false;

        if (!boundary->hit(r, Interval(rec1.t+0.0001, infinity), rec2))
            return false;

        if (rec1.t < Ray_t.min) rec1.t = Ray_t.min;
        if (rec2.t > Ray_t.max) rec2.t = Ray_t.max;

        if (rec1.t >= rec2.t)
            return false;

        if (rec1.t < 0)
            rec1.t = 0;

        auto Ray_length = r.d().norm();
        auto distance_inside_boundary = (rec2.t - rec1.t) * Ray_length;
        auto hit_distance = neg_inv_density * std::log(gen_random_double());

        if (hit_distance > distance_inside_boundary)
            return false;

        rec.t = rec1.t + hit_distance / Ray_length;
        rec.p = r.at(rec.t);

        rec.normal = vec4(1,0,0);  // arbitrary
        rec.front_face = true;     // also arbitrary
        rec.mat = phase_function;

        return true;
    }

    AABB bounding_box() const override { return boundary->bounding_box(); }

  private:
    shared_ptr<Hittable> boundary;
    double neg_inv_density;
    shared_ptr<Material> phase_function;
};

#endif