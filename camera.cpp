#include "camera.hpp"
#include <cmath>

camAxis::camAxis(const glm::vec3 &x, const glm::vec3 &y, const glm::vec3 &z)
    : forward(z), right(x), up(y) {}

Camera::Camera(const glm::vec3 &origin, const camAxis &axis, int fov, float aspectRatio)
    : position(origin), axis(axis), fov(fov), aspectRatio(aspectRatio) {}

void Camera::movePosition(const glm::vec3 &newPosition){
    position = newPosition;
}

void Camera::moveDirection(const glm::vec3 &newDirection){
    // TODO: direction change
}

void Camera::rotateCam(){
    // TODO: camera rotation
}

Ray Camera::generateRay(int pixelX, int pixelY, int imageWidth, int imageHeight) const{
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
