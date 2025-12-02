//==============================================================================================
// Originally written in 2016 by Peter Shirley <ptrshrl@gmail.com>
//
// To the extent possible under law, the author(s) have dedicated all copyright and related and
// neighboring rights to this software to the public domain worldwide. This software is
// distributed without any warranty.
//
// You should have received a copy (see file COPYING.txt) of the CC0 Public Domain Dedication
// along with this software. If not, see <http://creativecommons.org/publicdomain/zero/1.0/>.
//==============================================================================================

#include "rtweekend.h"

#include "camera.h"
#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"

#include <iostream>
#include <fstream>
#include <chrono>

int main() {
    hittable_list world;

    auto material_center = make_shared<lambertian>(color(0.7, 0.2, 0.2)); // Red
    auto material_left   = make_shared<lambertian>(color(0.7, 0.2, 0.2));
    auto material_right  = make_shared<lambertian>(color(0.7, 0.2, 0.2));

    world.add(make_shared<sphere>(point3( 0.0, 0.0, -3.0), 1.0, material_center));
    world.add(make_shared<sphere>(point3(-2.0, 0.0, -4.0), 1.0, material_left));
    world.add(make_shared<sphere>(point3( 2.0, 0.0, -4.0), 1.0, material_right));

    camera cam;

    cam.aspect_ratio      = 4.0 / 3.0;
    cam.image_width       = 800;
    cam.samples_per_pixel = 1;
    cam.max_depth         = 50;

    cam.vfov     = 90;
    cam.lookfrom = point3(0,0,0);
    cam.lookat   = point3(0,0,-1);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0.0;
    cam.focus_dist    = 1.0;

    // Redirect std::cout to a file
    std::ofstream outfile("image.ppm");
    std::streambuf *coutbuf = std::cout.rdbuf(); // Save old buf
    std::cout.rdbuf(outfile.rdbuf()); // Redirect std::cout to image.ppm

    auto start = std::chrono::high_resolution_clock::now();

    cam.render(world);

    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    
    std::cout.rdbuf(coutbuf); // Reset to standard output
    std::cout << "RTIOW Benchmark Time: " << duration.count() << " ms" << std::endl;
}
