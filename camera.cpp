#include "stdint.h"
#include "glm/glm.hpp"

using camVec3 = glm::vec3;
using lightVec3 = glm::vec3;

class Camera{
    private:
        camVec3 position;
        camVec3 direction;
        camVec3 upAxis;
        int fov;
        int aspectRatio;

    
    public:
        Camera(glm::vec3 origin, glm::vec3 direction, glm::vec3 upDir, int fov, int aspectRatio) : position(origin), direction(position), upAxis(upDir), fov(fov), aspectRatio(aspectRatio){}

        void movePosition(glm::vec3 newPosition){}
        void moveDirection(glm::vec3 newDirection){}
};
