#include "stdint.h"
#include "glm/glm.hpp"

using camVec3 = glm::vec3;
using lightVec3 = glm::vec3;

class Camera{
    private:
        camVec3 position;
        camVec3 direction;
    
    public:
        Camera(glm::vec3 origin, glm::vec3 direction) : position(origin), direction(position){}

        void movePosition(glm::vec3 newPosition){}
        void moveDirection(glm::vec3 newDirection){}
};

// init a camera