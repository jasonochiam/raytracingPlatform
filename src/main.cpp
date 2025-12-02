#include "camera.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <chrono>

#define IMAGEX 800
#define IMAGEY 600
#define FOV 90

int main(){
    /* 
    
    init scene
        - initialize camera
        - set up scene objects
        - set up light sources
        - specify image (aspect ratio, size, fov)

    loop over pixels in image (gpu)
        - generate rays from camera to each pixel

    trace each ray (gpu)
        - check for hits with scene objects
        - if no hit found:
            - color black (no light interaction)
        - else
            - compute color using lighting/shading/whatever
            - write pixel color to bit buffer (cpu)
            - once all pixels have a computation made, write image
    */

    camAxis mainAxis = camAxis(glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    Camera mainCam = Camera(glm::vec3(0.0f, 0.0f, 0.0f), mainAxis, FOV, (float)IMAGEX / IMAGEY);

    // Create a list of spheres
    std::vector<Sphere> spheres;
    spheres.push_back(Sphere(glm::vec3(0.0f, 0.0f, 3.0f), 1.0f));      // Center sphere
    spheres.push_back(Sphere(glm::vec3(2.0f, 0.0f, 4.0f), 1.0f));      // Right sphere
    spheres.push_back(Sphere(glm::vec3(-2.0f, 0.0f, 4.0f), 1.0f));     // Left sphere

    std::vector<unsigned char> framebuffer(IMAGEX * IMAGEY * 3, 0);

    // Light direction for simple Lambertian shading
    glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

    // Benchmark: Start Timer
    auto start = std::chrono::high_resolution_clock::now();

    for (int y = 0; y < IMAGEY; y++){
        for (int x = 0; x < IMAGEX; x++){
            Ray ray = mainCam.generateRay(x, y, IMAGEX, IMAGEY);
            
            float closestT = 10000.0f; // Initialize with a large value
            int hitSphereIndex = -1;

            // Check intersection with all spheres
            for (size_t i = 0; i < spheres.size(); i++) {
                float t;
                if (spheres[i].intersect(ray, t)) {
                    if (t < closestT) {
                        closestT = t;
                        hitSphereIndex = i;
                    }
                }
            }

            glm::vec3 color(0.0f);
            if (hitSphereIndex != -1){
                glm::vec3 hitPoint = ray.origin + ray.direction * closestT;
                glm::vec3 normal = glm::normalize(hitPoint - spheres[hitSphereIndex].getCenter());

                // Lambertian diffuse (clamped)
                float lambert = glm::max(glm::dot(normal, -lightDir), 0.0f);
                glm::vec3 baseColor(0.7f, 0.2f, 0.2f);
                color = baseColor * lambert;
            } else {
                // Background gradient
                float u = float(x) / float(IMAGEX);
                float v = float(y) / float(IMAGEY);
                color = glm::mix(glm::vec3(0.6f, 0.8f, 1.0f), glm::vec3(0.2f, 0.3f, 0.5f), v);
            }

            // Write to framebuffer (convert to 0-255)
            int idx = (y * IMAGEX + x) * 3;
            framebuffer[idx + 0] = (unsigned char)(glm::clamp(color.r, 0.0f, 1.0f) * 255.0f);
            framebuffer[idx + 1] = (unsigned char)(glm::clamp(color.g, 0.0f, 1.0f) * 255.0f);
            framebuffer[idx + 2] = (unsigned char)(glm::clamp(color.b, 0.0f, 1.0f) * 255.0f);
        }
    }

    // Write PPM
    std::FILE *out = std::fopen("output1.ppm", "wb");
    if (!out){
        std::cerr << "Failed to open output1.ppm for writing" << std::endl;
        return 1;
    }
    std::fprintf(out, "P6\n%d %d\n255\n", IMAGEX, IMAGEY);
    std::fwrite(framebuffer.data(), 1, framebuffer.size(), out);
    std::fclose(out);

    std::cout << "Wrote output1.ppm (" << IMAGEX << "x" << IMAGEY << ")" << std::endl;

    // Benchmark: Stop Timer
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
    std::cout << "My Engine Render Time: " << duration.count() << " ms" << std::endl;

    std::cout << "Opening output1.ppm in VS Code..." << std::endl;
        std::system("code output1.ppm");
}