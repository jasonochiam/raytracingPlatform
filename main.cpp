#include "camera.cpp"
#include "ray.cpp"
#include <vector>

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
    Camera mainCam = Camera(glm::vec3(0.0f, 0.0f, 0.0f), mainAxis, FOV, IMAGEX/IMAGEY);

    float pixelArray[IMAGEY][IMAGEX] = {0};

}