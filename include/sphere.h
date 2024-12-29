#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "onb.h"
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

        double pdf_value(const point4& origin, const vec4& dir) const override {
            // NOTE: Works only for stationary spheres!
            Hit rec;
            if (!this->hit(Ray(origin, dir), Interval(0.001, infinity), rec)) {
                return 0.0;
            }
            auto dist_squared = (center.at(0) - origin).norm2();
            auto cos_theta_max = std::sqrt(1-radius*radius/dist_squared);
            auto solid_angle = 2 * pi * (1 - cos_theta_max);

            return 1.0 / solid_angle;
        }
        
        vec4 random(const point4& origin) const override {
            vec4 dir = center.at(0) - origin;
            auto dist_squared = dir.norm2();
            ONB uvw(dir);
            return uvw.transform(random_to_sphere(radius, dist_squared));
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

        static vec4 random_to_sphere(double r, double dist_squared) {
            auto r1 = gen_random_double();
            auto r2 = gen_random_double();
            auto z  = 1 + r2*(std::sqrt(1-r*r/dist_squared) - 1);
            
            auto phi = 2 * pi * r1;
            auto x = std::cos(phi) * std::sqrt(1 - z * z);
            auto y = std::sin(phi) * std::sqrt(1 - z * z);

            return vec4(x,y,z);
        }
};

#endif