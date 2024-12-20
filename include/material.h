#ifndef MATERIAL_H
#define MATERIAL_H

#include "texture.h"
#include "hittable.h"

class Material {
    public:
        virtual ~Material() = default;
        virtual Color emitted(double u, double v, const point4& p) const {
            return Color(0,0,0); // black in the case for non-emitting materials
        } // u,v tuxture coordinates and 3D position 
        // scatter is the reflecting ray from hit point
        virtual bool scatter(
            const Ray& r_in,
            const Hit& rec,
            Color& attenuation,
            Ray& scattered
        ) const { return false; }
};

class Lambertian : public Material {
    public:
        Lambertian(const Color& albedo) : tex(make_shared<SolidColor>(albedo)) {}
        Lambertian(shared_ptr<Texture> tex) : tex(tex) {}
        bool scatter(
            const Ray& r_in, // ray going in
            const Hit& rec,  // store hit record data
            Color& attenuation, // fraction of light absorbed by material during scattering
            Ray& scattered)
            const override
        {
            auto scatter_dir = rec.normal + random_unit_vector();
            #ifdef DEBUG_MODE
                // std::cout << "Lambertian::scatter::direction(" << scatter_dir << ")\n";
                std::cout << "Lambertian::scatter::magnitude(" << scatter_dir.norm() << ")\n";
            #endif
            if (scatter_dir.near_zero()) {
                // if scatter direction is exactly opposite of the normal
                // then it sums to 0, leading to zero scatter direction
                // in this case, we simply return the normal
                scatter_dir = rec.normal;
            }
            scattered = Ray(rec.p, scatter_dir, r_in.time());
            attenuation = tex->value(rec.u, rec.v, rec.p);
            // std::cout << "atten (" << attenuation << ")\n";
            return true;
        }
    private:
        // Color albedo;
        shared_ptr<Texture> tex;
};

class Metal : public Material {
    public: 
        Metal(const Color& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1 ? fuzz : 1) {}
        // 'scatter'defines how an incoming ray interacts with material to produce outgoing ray
        // if diffuse, this is somewhat random, and if 1.0 albedo, fully reflects.
        // 'attenuation' is the scaling factor of light's intensity
        bool scatter(
                const Ray& r_in,
                const Hit& rec,
                Color& attenuation,
                Ray& scattered)
                const override
            {
                vec4 reflected = reflect(r_in.d(), rec.normal);
                reflected = unit_vector(reflected) + (fuzz * random_unit_vector());
                scattered = Ray(rec.p, reflected, r_in.time()); // hit point to outgoing ray
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

class DiffuseLight : public Material {
    public:
        DiffuseLight(shared_ptr<Texture> tex) : tex(tex) {}
        DiffuseLight(const Color& emit) : tex(make_shared<SolidColor>(emit)) {}

        Color emitted(double u, double v, const point4& p) const override {
            return tex->value(u,v,p);
        }

    private:
        shared_ptr<Texture> tex;
};

class Isotropic : public Material {
    public:
        Isotropic(const Color& albedo) : tex(make_shared<SolidColor>(albedo)) {}
        Isotropic(shared_ptr<Texture> albedo) : tex(tex) {}

        bool scatter(const Ray& r_in, const Hit& rec, Color& attenuation, Ray& scattered) const override {
            // scatter in a random direction
            scattered = Ray(rec.p, random_unit_vector(), r_in.time());
            attenuation = tex->value(rec.u, rec.v, rec.p);
            return true;
        }
    private:
        shared_ptr<Texture> tex;
};

#endif