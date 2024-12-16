#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "sphere.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"

// int main() {
//     HittableList world;

//     auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
//     auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
//     auto material_left   = make_shared<Dielectric>(1.50);
//     auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
//     auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

//     world.add(make_shared<Sphere>(point4( 0.0, -100.5, -1.0), 100.0, material_ground));
//     world.add(make_shared<Sphere>(point4( 0.0,    0.0, -1.2),   0.5, material_center));
//     world.add(make_shared<Sphere>(point4(-1.0,    0.0, -1.0),   0.5, material_left));
//     world.add(make_shared<Sphere>(point4(-1.0,    0.0, -1.0),   0.4, material_bubble));
//     world.add(make_shared<Sphere>(point4( 1.0,    0.0, -1.0),   0.5, material_right));

//     // auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
//     // auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
//     // // auto material_center   = make_shared<Dielectric>(1.00); 
//     // auto material_left   = make_shared<Dielectric>(1.50); 
//     // auto material_bubble = make_shared<Dielectric>(1.00/1.50); // hollow
//     // // auto material_left   = make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.3);
//     // auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

//     // // world.add(make_shared<Sphere>(point4(0,0,-1), 0.5));
//     // // world.add(make_shared<Sphere>(point4(0,-100.5,-1), 100));

//     // world.add(make_shared<Sphere>(point4( 0.0, -100.5, -1.0), 100.0, material_ground));
//     // world.add(make_shared<Sphere>(point4( 0.0,    0.0, -1.2),   0.5, material_center));
//     // world.add(make_shared<Sphere>(point4(-1.0,    0.0, -1.0),   0.5, material_left));
//     // world.add(make_shared<Sphere>(point4(-1.0,    0.0, -1.0),   0.4, material_bubble));
//     // world.add(make_shared<Sphere>(point4( 1.0,    0.0, -1.0),   0.5, material_right));

//     Camera cam;
//     // intrinsics
//     cam.aspect_ratio = 16.0 / 9.0;
//     cam.image_width  = 400;
//     cam.samples_per_pixel = 100;
//     cam.max_depth  = 50;
//     cam.fovy       = 20;
//     // extrinsics
//     cam.lookfrom = point4(-2, 2, 1);
//     cam.lookat   = point4(0, 0, -1);
//     cam.vup      = vec4(0,1,0);
//     // depth of field effect
//     cam.defocus_angle = 10.0;
//     cam.focus_dist = 3.4;
//     cam.render(world);
// }

// 14. Final Render
int main() {
    HittableList world;

    auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    world.add(make_shared<Sphere>(point4(0,-1000,0), 1000, ground_material));

    for (int a = -11; a < 11; a++) {
        for (int b = -11; b < 11; b++) {
            auto choose_mat = gen_random_double();
            point4 center(a + 0.9*gen_random_double(), 0.2, b + 0.9*gen_random_double());

            if ((center - point4(4, 0.2, 0)).norm() > 0.9) {
                shared_ptr<Material> Sphere_material;

                if (choose_mat < 0.8) {
                    // diffuse
                    auto albedo = Color::random() * Color::random();
                    Sphere_material = make_shared<Lambertian>(albedo);
                    auto center2 = center + vec4(0, gen_random_double(0, .5), 0);
                    world.add(make_shared<Sphere>(center, center2, 0.2, Sphere_material));
                } else if (choose_mat < 0.95) {
                    // Metal
                    auto albedo = Color::random(0.5, 1);
                    auto fuzz = gen_random_double(0, 0.5);
                    Sphere_material = make_shared<Metal>(albedo, fuzz);
                    world.add(make_shared<Sphere>(center, 0.2, Sphere_material));
                } else {
                    // glass
                    Sphere_material = make_shared<Dielectric>(1.5);
                    world.add(make_shared<Sphere>(center, 0.2, Sphere_material));
                }
            }
        }
    }

    auto material1 = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(point4(0, 1, 0), 1.0, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    world.add(make_shared<Sphere>(point4(-4, 1, 0), 1.0, material2));

    auto material3 = make_shared<Metal>(Color(0.7, 0.6, 0.5), 0.0);
    world.add(make_shared<Sphere>(point4(4, 1, 0), 1.0, material3));

    // structure objects as BVH
    world = HittableList(make_shared<BVH_node>(world));
    
    Camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fovy     = 20;
    cam.lookfrom = point4(13,2,3);
    cam.lookat   = point4(0,0,0);
    cam.vup      = vec4(0,1,0);

    cam.defocus_angle = 0.6;
    cam.focus_dist    = 10.0;

    cam.render(world);
}
