#ifndef CAMERA_H
#define CAMERA_H

#include "rtweekend.h"
#include "material.h"
#include <fstream>
#include <../src/part1/ppm2png.cpp>

const std::string OUT_FILENAME = "output";

std::vector<std::string> split(const std::string str, const std::string& delimiter) {
    std::vector<std::string> result;
    size_t start = 0;
    size_t end = str.find(delimiter);

    while (end != std::string::npos) {
        result.push_back(str.substr(start, end - start));
        start = end + delimiter.length();
        end = str.find(delimiter, start);
    }
    result.push_back(str.substr(start)); // Add the last token

    return result;
}

class Camera {
  public: 
    double aspect_ratio = 1.0; // width / height ratio
    int max_depth = 10; // max depth for recursive ray_color function
    int image_width = 100;
    int samples_per_pixel = 10; // random samples for each pixel
    Color background; // background color

    double fovy = 90;
    point4 lookfrom = point4(0,0,0);
    point4 lookat = point4(0,0,-1); // -z axis
    vec4 vup = vec4(0,1,0);
    
    double defocus_angle = 0; // variation angle of rays through each pixel
    double focus_dist = 10; // distance from cam center to perfect focus plane

    void render(const Hittable& world) {
        initialize();
        std::ofstream output_file(OUT_FILENAME + ".ppm");

        if (output_file.is_open()) {
            // Render
          output_file << "P3\n" << image_width << " " << image_height << "\n255\n";

          for (int j = 0; j < image_height; j++) {
              std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
              for (int i = 0; i < image_width; i++) {
                  Color pixel_color(0,0,0); // to be filled
                  
                  #ifdef DEBUG_MODE
                    std::cout << "(" << i << ", " << j << ")\n"; // pixel
                  #endif
                  for (int sample = 0; sample < samples_per_pixel; sample++) {
                    Ray r = get_ray(i, j);
                    pixel_color += ray_color(r, max_depth, world);
                  }
                  pixel_color *= (1.0 / samples_per_pixel);
                  write_color(output_file, pixel_color);
              }
          }
          output_file.close();
        } else {
            std::cerr << "Error opening file!\n";
        }
        // Convert ppm to png
        convertPPMtoPNG(OUT_FILENAME + ".ppm", OUT_FILENAME + ".png");
        std::clog << "\rDone.                 \n";
    }

  private:
    int image_height;
    point4 center;
    point4 pixel00_loc;
    vec4 pixel_delta_u;
    vec4 pixel_delta_v;
    vec4 u, v, w; // camera frame basis vectors
    vec4 defocus_disk_u;
    vec4 defocus_disk_v;

    void initialize() {
        // Image dim
        std::vector<int> resolution = calculate_res(aspect_ratio, image_width); // 1280x720
        image_width = resolution[0]; 
        image_height = resolution[1];
        center = lookfrom;
        // double focal_length = (lookfrom - lookat).norm();
        auto theta = degrees_to_radians(fovy);
        auto h = std::tan(theta / 2) * focus_dist; // height of half viewport (vertical)
        auto viewport_height = 2 * h;
        auto viewport_width  = viewport_height * (double(image_width)/image_height);

        w = unit_vector(lookfrom - lookat);
        u = unit_vector(cross(vup, w));
        v = cross(w, u);

        auto viewport_u = viewport_width * u;
        auto viewport_v = viewport_height * -v;

        pixel_delta_u = viewport_u / image_width;
        pixel_delta_v = viewport_v / image_height;

        // eye - length from viewport center
        auto viewport_upper_left = center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
        pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

        auto defocus_radius = focus_dist * std::tan(degrees_to_radians(defocus_angle / 2));
        defocus_disk_u = u * defocus_radius;
        defocus_disk_v = v * defocus_radius;
    }


    // std::vector<int> calculate_res(const std::string& aspect, int width) {
    //     std::vector<double> ratios(2);
    //     std::vector<std::string> xy = split(aspect, ":");
    //     ratios[0] = std::stoi(xy[0]);
    //     ratios[1] = std::stoi(xy[1]);
    //     auto aspect_ratio = ratios[0] / ratios[1];
    //     int image_height = int(width / aspect_ratio);
    //     image_height = (image_height < 1) ? 1 : image_height;

    //     return std::vector<int>{width, image_height};
    // }

    std::vector<int> calculate_res(const double aspect_ratio, int width) {
        int image_height = int(width / aspect_ratio);
        image_height = (image_height < 1) ? 1 : image_height;
        return std::vector<int>{width, image_height};
    }

    vec4 sample_square() const {
      // returns vector to random point in unit square (pixel size)
      return vec4(gen_random_double() - 0.5, gen_random_double() - 0.5, 0);
    }

    point4 defocus_disk_sample() const {
      auto p = random_in_unit_disk();
      return center + (p[0] * defocus_disk_u) + (p[1] * defocus_disk_v);
    }

    Ray get_ray(int i, int j) const {
      // creates camera rays towards pixel (i,j)
      auto offset = sample_square();
      auto pixel_sample = pixel00_loc
                        + ((i + offset.x()) * pixel_delta_u)
                        + ((j + offset.y()) * pixel_delta_v);

      auto ray_orig = (defocus_angle <= 0) ? center : defocus_disk_sample();
      auto ray_dir  = pixel_sample - ray_orig;
      auto ray_time = gen_random_double();
      return Ray(ray_orig, ray_dir, ray_time);
    }

    Color ray_color(const Ray& r, int depth, const Hittable& world) const {
        if (depth <= 0) return Color(0,0,0);

        Hit rec;
        // 0.001 on the interval to prevent "shadow acne"
        // where numerical approximations cause intersection error
        if (!world.hit(r, Interval(0.001, infinity), rec)) {
          return background;
        }

        #ifdef DEBUG_MODE
          std::cout << "HIT: " << rec.p << " || NORMAL: " << rec.normal << "\n";
        #endif

        Ray scattered;
        Color attenuation;
        Color color_from_emission = rec.mat->emitted(rec.u, rec.v, rec.p);

        // if ray produces a valid reflecting ray
        if (!rec.mat->scatter(r, rec, attenuation, scattered)) {
          // if material does not scatter (gets fully absorbed)
          return color_from_emission;
        }
        // emission is (0,0,0) if material is not emissive
        // recursively color ray from multiple scatters based on 'max_depth'
        Color color_from_scatter = attenuation * ray_color(scattered, depth - 1, world);
        return color_from_emission + color_from_scatter;

        // vec4 direction = random_on_hemisphere(rec.normal); // uniform scattering
        //vec4 direction = rec.normal + random_unit_vector(); // Lambertian
        // 0.5 for 50% diffuse (1.0 is white, 0.0 is black)
        // return reflectance * ray_color(Ray(rec.p, direction), depth - 1, world);

    }
};

#endif