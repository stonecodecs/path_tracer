#ifndef MATERIAL_H
#define MATERIAL_H

#include "hittable.h"

class Material {
    public:
        virtual ~Material() = default;
        virtual bool scatter(
            const Ray& r_in,
            const Hit& rec,
            Color& attenuation,
            Ray& scattered
        ) const { return false; }
};

class Lambertian : public Material {
    public:
        Lambertian(const Color& albedo) : albedo(albedo) {}
        bool scatter(
            const Ray& r_in,
            const Hit& rec,
            Color& attenuation,
            Ray& scattered)
            const override
        {
            auto scatter_dir = rec.normal + random_unit_vector();
            if (scatter_dir.near_zero()) {
                // if scatter direction is exactly opposite of the normal
                // then it sums to 0, leading to zero scatter direction
                // in this case, we simply return the normal
                scatter_dir = rec.normal;
            }
            scattered = Ray(rec.p, scatter_dir, r_in.time());
            attenuation = albedo;
            return true;
        }
    private:
        Color albedo;
};

class Metal : public Material {
    public: 
        Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}
        bool scatter(
                const Ray& r_in,
                const Hit& rec,
                Color& attenuation,
                Ray& scattered)
                const override
            {
                vec4 reflected = reflect(r_in.d(), rec.normal);
                reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
                scattered = Ray(rec.p, reflected, r_in.time());
                attenuation = albedo;
                return (dot(scattered.d(), rec.normal) > 0);
            }

    private:
        Color albedo;
        double fuzz;
};

class Dielectric : public Material {
    public:
        Dielectric(double index) : refraction_index(index) {};
        bool scatter(const Ray& r_in, const Hit& rec, Color& attenuation, Ray& scattered)
        const override {
            attenuation = Color(1.0, 1.0, 1.0);
            double ri = rec.front_face ? (1.0/refraction_index) : refraction_index;
            // renormalize due to numerical errors
            vec4 unit_direction = unit_vector(r_in.d());
            vec4 hit_normal = unit_vector(rec.normal);
            // vec4 dir = refract(unit_direction, rec.normal, ri);

            double cos_theta = std::fmin(dot(-unit_direction, hit_normal), 1.0);
            double sin_theta = std::sqrt(1.0 - cos_theta*cos_theta);

            bool cannot_refract = ri * sin_theta > 1.0;
            vec4 direction; // direction of the refracted/reflected ray

            if (cannot_refract || reflectance(cos_theta, ri) > gen_random_double()) {
                // must reflect case
                direction = reflect(unit_direction, hit_normal);
            } else {
                // can refract
                direction = refract(unit_direction, hit_normal, ri);
            }

            scattered = Ray(rec.p, direction, r_in.time());
            return true;
        }

    private:
        double refraction_index;

        static double reflectance(double cosine, double r_index) {
            // Schlick's approximation
            auto r0 = (1 - r_index) / (1 + r_index);
            r0 = r0 * r0;
            return r0 + (1 - r0) * std::pow((1 - cosine), 5);
        }

};

#endif