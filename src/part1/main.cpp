// #define DEBUG_MODE
#include "rtweekend.h"
#include "hittable.h"
#include "hittable_list.h"
#include "camera.h"
#include "material.h"
#include "bvh.h"
#include "texture.h"
#include "primitives.h"
#include "constant_medium.h"


void set_camera_settings(Camera& cam) {
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fovy     = 20;
    cam.lookfrom = point4(13,2,3);
    cam.lookat   = point4(0,0,0);
    cam.vup      = vec4(0,1,0);
    cam.defocus_angle = 0;
}

void part1full() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(Color(0.1, 0.2, 0.5));
    auto material_left   = make_shared<Dielectric>(1.50);
    auto material_bubble = make_shared<Dielectric>(1.00 / 1.50);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 1.0);

    world.add(make_shared<Sphere>(point4( 0.0, -100.5, -1.0), 100.0, material_ground));
    world.add(make_shared<Sphere>(point4( 0.0,    0.0, -1.2),   0.5, material_center));
    world.add(make_shared<Sphere>(point4(-1.0,    0.0, -1.0),   0.5, material_left));
    world.add(make_shared<Sphere>(point4(-1.0,    0.0, -1.0),   0.4, material_bubble));
    world.add(make_shared<Sphere>(point4( 1.0,    0.0, -1.0),   0.5, material_right));

    Camera cam;
    // intrinsics
    cam.aspect_ratio = 16.0 / 9.0;
    cam.image_width  = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth  = 50;
    cam.fovy       = 20;
    // extrinsics
    cam.lookfrom = point4(-2, 2, 1);
    cam.lookat   = point4(0, 0, -1);
    cam.vup      = vec4(0,1,0);
    // depth of field effect
    cam.defocus_angle = 10.0;
    cam.focus_dist = 3.4;
    cam.render(world, lights);
}

// 14. Final Render
void bouncing_spheres() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    // auto ground_material = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    auto checker = make_shared<CheckeredTexture>(0.32, Color(.2,.3,.1), Color(.9,.9,.9));
    world.add(make_shared<Sphere>(point4(0,-1000,0), 1000, make_shared<Lambertian>(checker)));

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

    cam.render(world, lights);
}

void checkered_spheres() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    auto checker = make_shared<CheckeredTexture>(0.32, Color(.2, .3, .1), Color(.9, .9, .9));

    world.add(make_shared<Sphere>(point4(0,-10, 0), 10, make_shared<Lambertian>(checker)));
    world.add(make_shared<Sphere>(point4(0, 10, 0), 10, make_shared<Lambertian>(checker)));

    Camera cam;
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fovy     = 20;
    cam.lookfrom = point4(13,2,3);
    cam.lookat   = point4(0,0,0);
    cam.vup      = vec4(0,1,0);
    cam.defocus_angle = 0;

    cam.render(world, lights);
}

void debug_spheres() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    auto R = std::cos(pi/4);

    auto material_left  = make_shared<Lambertian>(Color(0,0,1));
    auto material_right = make_shared<Lambertian>(Color(1,0,0));

    world.add(make_shared<Sphere>(point4(-R, 0, -1), R, material_left));
    world.add(make_shared<Sphere>(point4( R, 0, -1), R, material_right));

    world = HittableList(make_shared<BVH_node>(world));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = Color(0.7, 0.8, 1.0);

    cam.fovy = 90;

    cam.lookfrom = point4(0, 0, 0);
    cam.lookat   = point4(0, 0, -1);
    cam.vup      = vec4(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}

void earth() {
    auto earth_texture = make_shared<ImageTexture>("src/earthmap.jpg");
    auto earth_surface = make_shared<Lambertian>(earth_texture);
    auto globe = make_shared<Sphere>(point4(0,0,0), 2, earth_surface);
    auto empty_mat = shared_ptr<Material>();
    Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    Camera cam;
    // set_camera_settings(cam);
    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fovy     = 20;
    cam.lookfrom = point4(0,0,12);
    cam.lookat   = point4(0,0,0);
    cam.vup      = vec4(0,1,0);
    cam.defocus_angle = 0;
    cam.render(HittableList(globe), lights);
}

void perlin_spheres() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    auto perlin_texture = make_shared<NoiseTexture>(4);
    world.add(make_shared<Sphere>(point4(0,-1000,0), 1000, make_shared<Lambertian>(perlin_texture)));
    world.add(make_shared<Sphere>(point4(0,2,0), 2, make_shared<Lambertian>(perlin_texture)));

    Camera cam;
    set_camera_settings(cam);
    cam.render(world, lights);
}

void quads() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    // Materials
    auto left_red     = make_shared<Lambertian>(Color(1.0, 0.2, 0.2));
    auto back_green   = make_shared<Lambertian>(Color(0.2, 1.0, 0.2));
    auto right_blue   = make_shared<Lambertian>(Color(0.2, 0.2, 1.0));
    auto upper_orange = make_shared<Lambertian>(Color(1.0, 0.5, 0.0));
    auto lower_teal   = make_shared<Lambertian>(Color(0.2, 0.8, 0.8));

    // Quads
    world.add(make_shared<Quad>(point4(-3,-2, 5), vec4(0, 0,-4), vec4(0, 4, 0), left_red));
    world.add(make_shared<Quad>(point4(-2,-2, 0), vec4(4, 0, 0), vec4(0, 4, 0), back_green));
    world.add(make_shared<Quad>(point4( 3,-2, 1), vec4(0, 0, 4), vec4(0, 4, 0), right_blue));
    world.add(make_shared<Quad>(point4(-2, 3, 1), vec4(4, 0, 0), vec4(0, 0, 4), upper_orange));
    world.add(make_shared<Quad>(point4(-2,-3, 5), vec4(4, 0, 0), vec4(0, 0,-4), lower_teal));

    Camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;

    cam.fovy     = 80;
    cam.lookfrom = point4(0,0,9);
    cam.lookat   = point4(0,0,0);
    cam.vup      = vec4(0,1,0);

    cam.defocus_angle = 0;
    cam.background = Color(0.70, 0.80, 1.00);

    cam.render(world, lights);
}

void simple_light() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    auto pertext = make_shared<NoiseTexture>(4);
    world.add(make_shared<Sphere>(point4(0,-1000,0), 1000, make_shared<Lambertian>(pertext)));
    world.add(make_shared<Sphere>(point4(0,2,0), 2, make_shared<Lambertian>(pertext)));
    auto difflight = make_shared<DiffuseLight>(Color(4,4,4));
    world.add(make_shared<Sphere>(point4(0,7,0), 2, difflight));
    world.add(make_shared<Quad>(point4(3,1,-2), vec4(2,0,0), vec4(0,2,0), difflight));

    Camera cam;

    cam.aspect_ratio      = 16.0 / 9.0;
    cam.image_width       = 400;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = Color(0,0,0);

    cam.fovy     = 20;
    cam.lookfrom = point4(26,3,6);
    cam.lookat   = point4(0,2,0);
    cam.vup      = vec4(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}

void cornell_box() {
    HittableList world;

    auto red   = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(15, 15, 15));

    world.add(make_shared<Quad>(point4(555,0,0), vec4(0,555,0), vec4(0,0,555), green));
    world.add(make_shared<Quad>(point4(0,0,0), vec4(0,555,0), vec4(0,0,555), red));
    world.add(make_shared<Quad>(point4(343, 554, 332), vec4(-130,0,0), vec4(0,0,-105), light));
    world.add(make_shared<Quad>(point4(0,0,0), vec4(555,0,0), vec4(0,0,555), white));
    world.add(make_shared<Quad>(point4(555,555,555), vec4(-555,0,0), vec4(0,0,-555), white));
    world.add(make_shared<Quad>(point4(0,0,555), vec4(555,0,0), vec4(0,555,0), white));

    shared_ptr<Material> aluminum = make_shared<Metal>(Color(0.8, 0.85, 0.88), 0.0);
    shared_ptr<Hittable> box1 = box(point4(0,0,0), point4(165,330,165), white);
    box1 = make_shared<Rotate_y>(box1, 15);
    box1 = make_shared<Translate>(box1, vec4(265,0,295));
    world.add(box1);

    // shared_ptr<Hittable> box2 = box(point4(0,0,0), point4(165,165,165), white);
    // // shared_ptr<Hittable> box2 = make_shared<Sphere>(point4(85,85,85), 50, white);
    // box2 = make_shared<Rotate_y>(box2, -18);
    // box2 = make_shared<Translate>(box2, vec4(130,0,65));
    // world.add(box2);

    auto glass = make_shared<Dielectric>(1.5);
    world.add(make_shared<Sphere>(point4(190,90,190), 90, glass));

    auto empty_mat = shared_ptr<Material>();
    
    HittableList lights;
    lights.add(make_shared<Quad>(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat));
    lights.add(make_shared<Sphere>(point4(190, 90, 190), 90, empty_mat));
    world = HittableList(make_shared<BVH_node>(world));

    Camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 100;
    cam.max_depth         = 50;
    cam.background        = Color(0,0,0);

    cam.fovy     = 40;
    cam.lookfrom = point4(278, 278, -800);
    cam.lookat   = point4(278, 278, 0);
    cam.vup      = vec4(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}

void cornell_smoke() {
    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    auto red   = make_shared<Lambertian>(Color(.65, .05, .05));
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    auto green = make_shared<Lambertian>(Color(.12, .45, .15));
    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));

    world.add(make_shared<Quad>(point4(555,0,0), vec4(0,555,0), vec4(0,0,555), green));
    world.add(make_shared<Quad>(point4(0,0,0), vec4(0,555,0), vec4(0,0,555), red));
    world.add(make_shared<Quad>(point4(113,554,127), vec4(330,0,0), vec4(0,0,305), light));
    world.add(make_shared<Quad>(point4(0,555,0), vec4(555,0,0), vec4(0,0,555), white));
    world.add(make_shared<Quad>(point4(0,0,0), vec4(555,0,0), vec4(0,0,555), white));
    world.add(make_shared<Quad>(point4(0,0,555), vec4(555,0,0), vec4(0,555,0), white));

    shared_ptr<Hittable> box1 = box(point4(0,0,0), point4(165,330,165), white);
    box1 = make_shared<Rotate_y>(box1, 15);
    box1 = make_shared<Translate>(box1, vec4(265,0,295));

    shared_ptr<Hittable> box2 = box(point4(0,0,0), point4(165,165,165), white);
    box2 = make_shared<Rotate_y>(box2, -18);
    box2 = make_shared<Translate>(box2, vec4(130,0,65));

    world.add(make_shared<ConstantMedium>(box1, 0.01, Color(0,0,0)));
    world.add(make_shared<ConstantMedium>(box2, 0.01, Color(1,1,1)));

    Camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background        = Color(0,0,0);

    cam.fovy     = 40;
    cam.lookfrom = point4(278, 278, -800);
    cam.lookat   = point4(278, 278, 0);
    cam.vup      = vec4(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
    HittableList boxes1;
    auto ground = make_shared<Lambertian>(Color(0.48, 0.83, 0.53));

    int boxes_per_side = 20;
    for (int i = 0; i < boxes_per_side; i++) {
        for (int j = 0; j < boxes_per_side; j++) {
            auto w = 100.0;
            auto x0 = -1000.0 + i*w;
            auto z0 = -1000.0 + j*w;
            auto y0 = 0.0;
            auto x1 = x0 + w;
            auto y1 = gen_random_double(1,101);
            auto z1 = z0 + w;

            boxes1.add(box(point4(x0,y0,z0), point4(x1,y1,z1), ground));
        }
    }

    HittableList world;
auto empty_mat = shared_ptr<Material>();
Quad lights(point4(343,554,332), vec4(-130,0,0), vec4(0,0,-105), empty_mat);


    world.add(make_shared<BVH_node>(boxes1));

    auto light = make_shared<DiffuseLight>(Color(7, 7, 7));
    world.add(make_shared<Quad>(point4(123,554,147), vec4(300,0,0), vec4(0,0,265), light));

    auto center1 = point4(400, 400, 200);
    auto center2 = center1 + vec4(30,0,0);
    auto sphere_material = make_shared<Lambertian>(Color(0.7, 0.3, 0.1));
    world.add(make_shared<Sphere>(center1, center2, 50, sphere_material));

    world.add(make_shared<Sphere>(point4(260, 150, 45), 50, make_shared<Dielectric>(1.5)));
    world.add(make_shared<Sphere>(
        point4(0, 150, 145), 50, make_shared<Metal>(Color(0.8, 0.8, 0.9), 1.0)
    ));

    auto boundary = make_shared<Sphere>(point4(360,150,145), 70, make_shared<Dielectric>(1.5));
    world.add(boundary);
    world.add(make_shared<ConstantMedium>(boundary, 0.2, Color(0.2, 0.4, 0.9)));
    boundary = make_shared<Sphere>(point4(0,0,0), 5000, make_shared<Dielectric>(1.5));
    world.add(make_shared<ConstantMedium>(boundary, .0001, Color(1,1,1)));

    auto emat = make_shared<Lambertian>(make_shared<ImageTexture>("src/earthmap.jpg"));
    world.add(make_shared<Sphere>(point4(400,200,400), 100, emat));
    auto pertext = make_shared<NoiseTexture>(0.2);
    world.add(make_shared<Sphere>(point4(220,280,300), 80, make_shared<Lambertian>(pertext)));

    HittableList boxes2;
    auto white = make_shared<Lambertian>(Color(.73, .73, .73));
    int ns = 1000;
    for (int j = 0; j < ns; j++) {
        boxes2.add(make_shared<Sphere>(point4::random(0,165), 10, white));
    }

    world.add(make_shared<Translate>(
        make_shared<Rotate_y>(
            make_shared<BVH_node>(boxes2), 15),
            vec4(-100,270,395)
        )
    );

    Camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = image_width;
    cam.samples_per_pixel = samples_per_pixel;
    cam.max_depth         = max_depth;
    cam.background        = Color(0,0,0);

    cam.fovy     = 40;
    cam.lookfrom = point4(478, 278, -600);
    cam.lookat   = point4(278, 278, 0);
    cam.vup      = vec4(0,1,0);

    cam.defocus_angle = 0;

    cam.render(world, lights);
}

int main() {
    int select = 7;
    switch(select) {
        case 1:
            bouncing_spheres();
            break;
        case 2:
            checkered_spheres();
            break;
        case 3:
            earth();
            break;
        case 4:
            perlin_spheres();
            break;
        case 5:
            quads();
            break;
        case 6:
            simple_light();
            break;
        case 7:
            cornell_box();
            break;
        case 8:
            cornell_smoke();
            break;
        case 9: // part 2 final render
            final_scene(500, 300, 8);
            break;
        default:
            std::cout << "Loading debug spheres...\n";
            debug_spheres();
            break;
    }
}
