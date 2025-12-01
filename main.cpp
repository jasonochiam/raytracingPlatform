#include "camera.hpp"
#include "ray.hpp"
#include "sphere.hpp"
#include <vector>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <string>
#include <SDL2/SDL.h>

#define IMAGEX 800
#define IMAGEY 600
#define FOV 90

int main(){
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL could not initialize! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("Ray Tracer", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, IMAGEX, IMAGEY, SDL_WINDOW_SHOWN);
    if (window == NULL) {
        std::cerr << "Window could not be created! SDL_Error: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_Texture* texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB24, SDL_TEXTUREACCESS_STREAMING, IMAGEX, IMAGEY);

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

    bool quit = false;
    SDL_Event e;

    float speed = 0.1f; // Movement speed

    // FPS Counting variables
    Uint32 lastTime = SDL_GetTicks();
    int frameCount = 0;

    while (!quit) {
        while (SDL_PollEvent(&e) != 0) {
            if (e.type == SDL_QUIT) {
                quit = true;
            }
        }

        // Handle Keyboard Input
        const Uint8* currentKeyStates = SDL_GetKeyboardState(NULL);
        glm::vec3 moveDir(0.0f);

        if (currentKeyStates[SDL_SCANCODE_W]) moveDir += mainAxis.getForward();
        if (currentKeyStates[SDL_SCANCODE_S]) moveDir -= mainAxis.getForward();
        if (currentKeyStates[SDL_SCANCODE_A]) moveDir -= mainAxis.getRight();
        if (currentKeyStates[SDL_SCANCODE_D]) moveDir += mainAxis.getRight();
        if (currentKeyStates[SDL_SCANCODE_Q]) moveDir -= mainAxis.getUp();
        if (currentKeyStates[SDL_SCANCODE_E]) moveDir += mainAxis.getUp();

        if (glm::length(moveDir) > 0.0f) {
            moveDir = glm::normalize(moveDir) * speed;
            mainCam.movePosition(mainCam.getPosition() + moveDir);
        }

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

        SDL_UpdateTexture(texture, NULL, framebuffer.data(), IMAGEX * 3);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);

        // Calculate and display FPS
        frameCount++;
        Uint32 currentTime = SDL_GetTicks();
        if (currentTime - lastTime >= 1000) {
            std::string title = "Ray Tracer - FPS: " + std::to_string(frameCount);
            SDL_SetWindowTitle(window, title.c_str());
            frameCount = 0;
            lastTime = currentTime;
        }
    }

    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}