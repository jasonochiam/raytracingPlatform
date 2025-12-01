#ifndef CAMERA_HPP
#define CAMERA_HPP

#include "glm/glm.hpp"
#include "ray.hpp"
#include <cmath>

using camVec3 = glm::vec3;

class camAxis{
    private:
        camVec3 forward;
        camVec3 right;
        camVec3 up;

    public:
        camAxis(const glm::vec3 &x, const glm::vec3 &y, const glm::vec3 &z);

        const camVec3 &getForward() const { return forward; }
        const camVec3 &getRight() const { return right; }
        const camVec3 &getUp() const { return up; }
};

class Camera{
    private:
        camVec3 position;
        camAxis axis;
        int fov;
        float aspectRatio;

    public:
        Camera(const glm::vec3 &origin, const camAxis &axis, int fov, float aspectRatio);

        void movePosition(const glm::vec3 &newPosition);
        void moveDirection(const glm::vec3 &newDirection);
        void rotateCam();
        const glm::vec3 &getPosition() const { return position; }
        Ray generateRay(int pixelX, int pixelY, int imageWidth, int imageHeight) const;
};

#endif // CAMERA_HPP