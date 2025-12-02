#include "camera.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include <vector>
#include <iostream>

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

    Sphere testSphere(glm::vec3(0.0f, 0.0f, 3.0f), 1.0f);
    std::vector<unsigned char> framebuffer(IMAGEX * IMAGEY * 3, 0);

    // Light direction for simple Lambertian shading
    glm::vec3 lightDir = glm::normalize(glm::vec3(1.0f, 1.0f, 1.0f));

    for (int y = 0; y < IMAGEY; y++){
        for (int x = 0; x < IMAGEX; x++){
            Ray ray = mainCam.generateRay(x, y, IMAGEX, IMAGEY);
            float t;
            bool hit = testSphere.intersect(ray, t);

            glm::vec3 color(0.0f);
            if (hit){
                glm::vec3 hitPoint = ray.origin + ray.direction * t;
                glm::vec3 normal = glm::normalize(hitPoint - testSphere.getCenter());

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
}