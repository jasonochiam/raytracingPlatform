#include "stdint.h"
#include "glm/glm.hpp"

using camVec3 = glm::vec3;
using lightVec3 = glm::vec3;

class camAxis{
    private:
        camVec3 forward;
        camVec3 right;
        camVec3 up;
    
    public:
        camAxis(glm::vec3 x, glm::vec3 y, glm::vec3 z) : forward(z), right(x), up(y){}
};

class Camera{
    private:
        camVec3 position;
        camAxis axis;
        int fov;
        float aspectRatio;

    public:
        Camera(glm::vec3 origin, camAxis axis, int fov, int aspectRatio) : position(origin), axis(axis), fov(fov), aspectRatio(aspectRatio){}

        void movePosition(glm::vec3 newPosition){}
        void moveDirection(glm::vec3 newDirection){}
        void rotateCam(){}
};
