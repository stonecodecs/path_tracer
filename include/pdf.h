#ifndef PDF_H
#define PDF_H

#include "hittable_list.h"
#include "onb.h"

class PDF {
    public:
        virtual ~PDF() {}
        virtual double value(const vec4& dir) const = 0;
        virtual vec4 generate() const = 0;
};

class SpherePDF : public PDF { // "complete isotropic"
    public:
        SpherePDF() {}
        double value(const vec4& dir) const override {
            return 1 / (4 * pi);
        }

        vec4 generate() const override {
            return random_unit_vector();
        }   
};

class CosinePDF : public PDF { // "higher prob. near normal"
    public:
        CosinePDF(const vec4& w) : uvw(w) {}
        double value(const vec4& dir) const override {
            return std::fmax(
                0,
                dot(unit_vector(dir), uvw.w()) / pi
            );
        }

        vec4 generate() const override {
            return uvw.transform(random_cosine_direction());
        }   
    private:
        ONB uvw;
};

class HittablePDF : public PDF {
    public:
        HittablePDF(const Hittable& objects, const point4& origin)
        : objects(objects), origin(origin) {}

        double value(const vec4& dir) const override {
            return objects.pdf_value(origin, dir);
        }

        vec4 generate() const override {
            return objects.random(origin);
        }
    
    private:
        const Hittable& objects;
        point4 origin;
};

class MixturePDF : public PDF {
    public:
        MixturePDF(shared_ptr<PDF> p0, shared_ptr<PDF> p1) {
            p[0] = p0;
            p[1] = p1;
        }

        double value(const vec4& dir) const override {
            return 0.5 * p[0]->value(dir) + 0.5 * p[1]->value(dir);
        }

        vec4 generate() const override {
            // "half the time, sample from Light and other from Surface"
            if (gen_random_double() < 0.5) {
                return p[0]->generate();
            } else {
                return p[1]->generate();
            }
        }
    private:
        shared_ptr<PDF> p[2];

};


#endif