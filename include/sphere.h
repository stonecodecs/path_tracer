#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
// #include "vec4.h"

class Sphere : public Hittable {
    public:
        Sphere(const point4& static_center, double r, shared_ptr<Material> mat)
         : center(static_center, vec4(0,0,0)), radius(std::fmax(0,r)), mat(mat) {
            // sphere extremes are the bounds of the cube that embeds the sphere
            auto rvec = vec4(radius,radius,radius);
            bbox = AABB(static_center - rvec, static_center + rvec);
         }

        // center2 - center1 is the vector that goes from c1 to c2
        Sphere(const point4& center1, const point4& center2,
               double r, shared_ptr<Material> mat)
         : center(center1, center2 - center1), radius(std::fmax(0,r)), mat(mat) {
            auto rvec = vec4(radius,radius,radius);
            AABB box1(center.at(0) - rvec, center.at(0) + rvec);
            AABB box2(center.at(1) - rvec, center.at(1) + rvec);
            bbox = AABB(box1, box2); // creates box interval for all 't' [0, 1]
         }
        
        bool hit(const Ray& r, Interval ray_t, Hit& rec) const override {
            // finds intersections for a certain snapshot of a scene @ time 't'
            point4 current_center = center.at(r.time()); 
            vec4 oc = current_center - r.o();
            auto a = r.d().norm2();
            auto h = dot(r.d(), oc);
            auto c = oc.norm2() - radius*radius;

            auto discriminant = h*h - a*c;
            if (discriminant < 0)
                return false;

            auto sqrtd = std::sqrt(discriminant);

            // Find the nearest root that lies in the acceptable range.
            auto root = (h - sqrtd) / a;
            if (!ray_t.surrounds(root)) {
                root = (h + sqrtd) / a;
                if (!ray_t.surrounds(root))
                    return false;
            }

            // fill out hit record
            rec.t = root;
            rec.p = r.at(rec.t);
            // normal unit vec from center sphere to hit point
            vec4 normal_out = (rec.p - current_center) / radius;
            rec.mat = mat;
            rec.set_face_normal(r, normal_out);
            get_sphere_uv(normal_out, rec.u, rec.v);
            return true;
        }

        AABB bounding_box() const override { return bbox; }
    private:
        //point4 center;
        Ray center;
        double radius;
        shared_ptr<Material> mat;
        AABB bbox;

        static void get_sphere_uv(const point4& p, double& u, double& v) {
            // p: point on unit sphere 
            // u,v: normalized [0,1] mapping from spherical coords

            auto theta = std::acos(-p.y());
            auto phi = std::atan2(-p.z(), p.x()) + pi;
            u = phi / (2 * pi);
            v = theta / pi;
        }
};

#endif