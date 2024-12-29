#ifndef COLOR_H
#define COLOR_H

#include "interval.h"
#include "vec4.h"
using Color = vec4;

inline double linear_to_gamma(double linear_component) {
    if (linear_component > 0) {
        return std::sqrt(linear_component);
    }
    return 0;
}

// technically, RGB (can extend 'a' later)
void write_color(std::ostream& out, const Color& pixel_color) {
    auto r = pixel_color.x();
    auto g = pixel_color.y();
    auto b = pixel_color.z();
    // auto a = pixel_color.w();

    // replace NaN with 0 (NaN != NaN) to get rid of shadow acne
    if (r != r) r = 0.0;
    if (g != g) g = 0.0;
    if (b != b) b = 0.0;

    r = linear_to_gamma(r);
    g = linear_to_gamma(g);
    b = linear_to_gamma(b);

    static const Interval intensity(0.0, 0.999);

    // Translate the [0,1] component values to the byte range [0,255].
    int rbyte = int(256 * intensity.clamp(r));
    int gbyte = int(256 * intensity.clamp(g));
    int bbyte = int(256 * intensity.clamp(b));
    // int abtype = int(0.0); // technically ppm has no support, will ignore

    // Write out the pixel color components.
    out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';
}

#endif