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
    // for texture mapping, need u and v
    double u;
    double v;

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
            Interval Ray_t,
            Hit& hit_record) const = 0;
        virtual AABB bounding_box() const = 0;
        virtual double pdf_value(const point4& origin, const vec4& dir) const {
            return 0.0;
        }
        virtual vec4 random(const point4& origin) const {
            return vec4(1,0,0);
        }
};

class Translate : public Hittable {
    public:
        Translate(shared_ptr<Hittable> object, const vec4& offset)
        : object(object), offset(offset) {
            bbox = object->bounding_box() + offset;
        }
        bool hit(const Ray& r, Interval Ray_t, Hit& rec) const override {
            // move Ray backwards by the offset
            Ray offset_r(r.o() - offset, r.d(), r.time());

            // determine if intersection exists along offset Ray
            if(!object->hit(offset_r, Ray_t, rec)) {
                return false;
            }

            // move intersection point forwards by offset
            rec.p += offset;
            return true;
        }
        AABB bounding_box() const override { return bbox; }
    private:
        shared_ptr<Hittable> object;
        vec4 offset;
        AABB bbox;
};

// returns Ray w.r.t object frame for both o and direction
std::vector<vec4> to_object_frame(const Ray& r, double cos_theta, double sin_theta, int axis) {
    point4 o;
    point4 direction;

    if (axis == 0) {
        o = point4(
            r.o().x(),
            ( cos_theta * r.o().y()) + (sin_theta * r.o().z()),
            (-sin_theta * r.o().y()) + (cos_theta * r.o().z())
        );
        direction = point4(
            r.d().x(),
            ( cos_theta * r.d().y()) + (sin_theta * r.d().z()),
            (-sin_theta * r.d().y()) + (cos_theta * r.d().z())
        );
    }
    else if (axis == 1) {

        o = point4(
            (cos_theta * r.o().x()) + (-sin_theta * r.o().z()),
            r.o().y(),
            (sin_theta * r.o().x()) + (cos_theta * r.o().z())
        );
        direction = point4(
            (cos_theta * r.d().x()) + (-sin_theta * r.d().z()),
            r.d().y(),
            (sin_theta * r.d().x()) + (cos_theta * r.d().z())
        );
    } else {
    
        o = point4(
            ( cos_theta * r.o().x()) + (sin_theta * r.o().y()),
            (-sin_theta * r.o().x()) + (cos_theta * r.o().y()),
            r.o().z()
        );
        direction = point4(
            ( cos_theta * r.d().x()) + (sin_theta * r.d().y()),
            (-sin_theta * r.d().x()) + (cos_theta * r.d().y()),
            r.d().z()
        );
    }

    return {o, direction};
}

std::vector<vec4> to_world_frame(const Hit& rec, double cos_theta, double sin_theta, int axis) {
    point4 o; // hit point
    point4 n; // normal

    if (axis == 0) { // X

        o = point4(
            rec.p.x(),
            ( cos_theta * rec.p.y()) + (-sin_theta * rec.p.z()),
            (sin_theta * rec.p.y()) + (cos_theta * rec.p.z())
        );
        n = point4(
            rec.normal.x(),
            ( cos_theta * rec.normal.y()) + (-sin_theta * rec.normal.z()),
            (sin_theta * rec.normal.y()) + (cos_theta * rec.normal.z())
        );
    }
    else if (axis == 1) { // Y
        o = point4(
            (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
            rec.p.y(),
            (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
        );
        n = point4(
            (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
            rec.normal.y(),
            (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
        );
    } else { // Z

        o = point4(
            ( cos_theta * rec.p.x()) + (-sin_theta * rec.p.y()),
            (sin_theta * rec.p.x()) + (cos_theta * rec.p.y()),
            rec.p.z()
        );
        n = point4(
            ( cos_theta * rec.normal.x()) + (-sin_theta * rec.normal.y()),
            (sin_theta * rec.normal.x()) + (cos_theta * rec.normal.y()),
            rec.normal.z()
        );
    }

    return {o, unit_vector(n)};
}

class Rotate : public Hittable {
    public:
        Rotate(shared_ptr<Hittable> obj, double angle, int axis) : object(obj), axis(axis) {
            auto radians = degrees_to_radians(angle);
            sin_theta = std::sin(radians);
            cos_theta = std::cos(radians);
            bbox = object->bounding_box();

            point4 min( infinity,  infinity,  infinity);
            point4 max(-infinity, -infinity, -infinity);

            for (int i = 0; i < 2; i++) {
                for (int j = 0; j < 2; j++) {
                    for (int k = 0; k < 2; k++) {
                        auto x = i * bbox.x.max + (1-i) * bbox.x.min;
                        auto y = j * bbox.y.max + (1-j) * bbox.y.min;
                        auto z = k * bbox.z.max + (1-k) * bbox.z.min;

                        auto new_x =  cos_theta * x + sin_theta * z;
                        auto new_z = -sin_theta * x + cos_theta * z;

                        vec4 test_vec(new_x, y, new_z); // rotated

                        for (int c = 0; c < 3; c++) {
                            min[c] = std::fmin(min[c], test_vec[c]);
                            max[c] = std::fmax(max[c], test_vec[c]);
                        }

                    }
                }
            }

            bbox = AABB(min, max);
        }
        bool hit(const Ray& r, Interval Ray_t, Hit& rec) const override {
            // transform Ray from world to object space (inverse transform)
            // meaning, "rotate by -theta"
            std::vector<vec4> r_wrt_object = to_object_frame(
                r, cos_theta, sin_theta, axis
            );

            auto origin = r_wrt_object[0];
            auto direction = r_wrt_object[1];
            
            Ray rotated_ray(origin, direction, r.time());

            // check if intersection exists in obj space
            if (!object->hit(rotated_ray, Ray_t, rec)) {
                return false;
            }

            std::vector<vec4> r_wrt_world = to_world_frame(
                rec, cos_theta, sin_theta, axis
            );            

            // back to world space
            rec.p = r_wrt_world[0];
            rec.normal = r_wrt_world[1]; // for non-uniform scaling, need to apply inverse transpose

            return true;
        }
        AABB bounding_box() const override { return bbox; }
    private:
        shared_ptr<Hittable> object;
        double sin_theta;
        double cos_theta;
        AABB bbox;
        int axis; // [0, 2] corresponding to x,y,z axes
};

class Rotate_y : public Hittable {
  public:
    Rotate_y(shared_ptr<Hittable> object, double angle) : object(object) {
        auto radians = degrees_to_radians(angle);
        sin_theta = std::sin(radians);
        cos_theta = std::cos(radians);
        bbox = object->bounding_box();

        point4 min( infinity,  infinity,  infinity);
        point4 max(-infinity, -infinity, -infinity);

        for (int i = 0; i < 2; i++) {
            for (int j = 0; j < 2; j++) {
                for (int k = 0; k < 2; k++) {
                    auto x = i*bbox.x.max + (1-i)*bbox.x.min;
                    auto y = j*bbox.y.max + (1-j)*bbox.y.min;
                    auto z = k*bbox.z.max + (1-k)*bbox.z.min;

                    auto newx =  cos_theta*x + sin_theta*z;
                    auto newz = -sin_theta*x + cos_theta*z;

                    vec4 tester(newx, y, newz);

                    for (int c = 0; c < 3; c++) {
                        min[c] = std::fmin(min[c], tester[c]);
                        max[c] = std::fmax(max[c], tester[c]);
                    }
                }
            }
        }

        bbox = AABB(min, max);
    }
    bool hit(const Ray& r, Interval Ray_t, Hit& rec) const override {

        // Transform the Ray from world space to object space.

        auto o = point4(
            (cos_theta * r.o().x()) - (sin_theta * r.o().z()),
            r.o().y(),
            (sin_theta * r.o().x()) + (cos_theta * r.o().z())
        );

        auto direction = vec4(
            (cos_theta * r.d().x()) - (sin_theta * r.d().z()),
            r.d().y(),
            (sin_theta * r.d().x()) + (cos_theta * r.d().z())
        );

        Ray rotated_r(o, direction, r.time());

        // Determine whether an intersection exists in object space (and if so, where).

        if (!object->hit(rotated_r, Ray_t, rec))
            return false;

        // Transform the intersection from object space back to world space.

        rec.p = point4(
            (cos_theta * rec.p.x()) + (sin_theta * rec.p.z()),
            rec.p.y(),
            (-sin_theta * rec.p.x()) + (cos_theta * rec.p.z())
        );

        rec.normal = vec4(
            (cos_theta * rec.normal.x()) + (sin_theta * rec.normal.z()),
            rec.normal.y(),
            (-sin_theta * rec.normal.x()) + (cos_theta * rec.normal.z())
        );

        return true;
    }

    AABB bounding_box() const override { return bbox; }

    private:
        shared_ptr<Hittable> object;
        double sin_theta;
        double cos_theta;
        AABB bbox;
};

#endif