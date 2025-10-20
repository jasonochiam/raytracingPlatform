#ifndef CAMERA_H
#define CAMERA_H

#include "stdint.h"
#include "glm/glm.hpp"
#include "ray.cpp"
#include <cmath>

using camVec3 = glm::vec3;
using lightVec3 = glm::vec3;

class camAxis{
    private:
        camVec3 forward;
        camVec3 right;
        camVec3 up;
    
    public:
        camAxis(glm::vec3 x, glm::vec3 y, glm::vec3 z) : forward(z), right(x), up(y){}

        camVec3 getForward() const { return forward; }
        camVec3 getRight() const { return right; }
        camVec3 getUp() const { return up; }
};

class Camera{
    private:
        camVec3 position;
        camAxis axis;
        int fov;
        float aspectRatio;

    public:
        Camera(glm::vec3 origin, camAxis axis, int fov, float aspectRatio) : position(origin), axis(axis), fov(fov), aspectRatio(aspectRatio) {}

        void movePosition(glm::vec3 newPosition){}
        void moveDirection(glm::vec3 newDirection){}
        void rotateCam(){}
        Ray generateRay(int pixelX, int pixelY, int imageWidth, int imageHeight){
            // pixel coordinates -> NDC in [-1,1], (0,0) at center
            float ndcX = (2.0f * (pixelX + 0.5f) / imageWidth) - 1.0f;
            float ndcY = 1.0f - (2.0f * (pixelY + 0.5f) / imageHeight);
            ndcX *= aspectRatio;

            float fovRadians = fov * (M_PI / 180.0f);
            float scale = tan(fovRadians / 2.0f);
            ndcX *= scale;
            ndcY *= scale;

            glm::vec3 rayDirection = ndcX * axis.getRight() +
                                    ndcY * axis.getUp() +
                                    axis.getForward();

            return Ray(position, rayDirection);
        }
};
#endif
