#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
#include "rtw_stb_image.h"
#include "perlin.h"

class Texture {
    public:
        virtual ~Texture() = default;
        virtual Color value(double u, double v, const point4& p) const = 0;
        // u, v texture coords, p is hit point
};

class SolidColor : public Texture {
    public:
        SolidColor(const Color& albedo) : albedo(albedo) {}
        SolidColor(double r, double g, double b) : SolidColor(Color(r,g,b)) {}
        Color value(double u, double v, const point4& p) const override {
            return albedo;
        }  
    private:
        Color albedo;
};

class CheckeredTexture : public Texture {
    public:
        CheckeredTexture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
            : inv_scale(1.0 / scale), even(even), odd(odd) {}
        CheckeredTexture(double scale, const Color& c1, const Color& c2)
            : CheckeredTexture(scale, make_shared<SolidColor>(c1), make_shared<SolidColor>(c2)) {}

        Color value(double u, double v, const point4& p) const override {
            auto x = int(std::floor(inv_scale * p.x()));
            auto y = int(std::floor(inv_scale * p.y()));
            auto z = int(std::floor(inv_scale * p.z()));
            bool isEven = (x + y + z) % 2 == 0;

            return isEven ? even->value(u, v, p) : odd->value(u, v, p);
        }
    private:
        double inv_scale;
        shared_ptr<Texture> even;
        shared_ptr<Texture> odd;
};

class ImageTexture : public Texture {
    public: 
        ImageTexture(const char* filename) : image(filename) {}

        Color value(double u, double v, const point4& p) const override {
            // no texture data, default color fill
            if(image.height() <= 0) return Color(0,1,1);
            u = Interval(0,1).clamp(u);
            v = 1.0 - Interval(0,1).clamp(v); // flip 'v' to image coordinates
            // if not, the texture is upside down

            auto i = int(u * image.width());
            auto j = int(v * image.height());
            auto pixel = image.pixel_data(i,j); // 0-255, need normalize

            auto color_scale = 1.0 / 255.0;
            return Color(
                color_scale*pixel[0],
                color_scale*pixel[1],
                color_scale*pixel[2]);
        }
    private:
        rtw_image image;
};

class NoiseTexture : public Texture {
    public:
        NoiseTexture() {}
        NoiseTexture(double scale) : scale(scale) {}
        Color value(double u, double v, const point4& p) const override {
            //return Color(1,1,1) * (1.0 + noise.noise(scale * p)) * 0.5;
            // why 0.5? => Perlin interpolation can give negative values
            // linear_to_gamma() expects only positive
            // so 0.5 makes it so that the range is back to [0,1]
            
            //return Color(1,1,1) * noise.turb(p, 7);
            // make color proportional to something like a sin function
            // use turbulenec to adjust the phase
            return Color(0.5,0.5,0.5) * (1 + std::sin(scale * p.z() + 10 * noise.turb(p ,7)));
        }
    private:
        Perlin noise;
        double scale; // frequency
};

#endif