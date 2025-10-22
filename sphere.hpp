#ifndef SPHERE_H
#define SPHERE_H

#include "glm/glm.hpp"
#include "ray.hpp"
#include <cmath>

class Sphere {
    private:
        glm::vec3 center;
        float radius;

    public:
        Sphere(const glm::vec3 &c, float r) : center(c), radius(r) {}

        // Getters for center and radius
        const glm::vec3 &getCenter() const { return center; }
        float getRadius() const { return radius; }

        bool intersect(const Ray &ray, float &t) const {
            glm::vec3 oc = ray.origin - center;
            float a = glm::dot(ray.direction, ray.direction);
            float b = 2.0f * glm::dot(oc, ray.direction);
            float c = glm::dot(oc, oc) - radius * radius;
            float discriminant = b * b - 4 * a * c;
            if (discriminant < 0.0f)
                return false; // miss

            float sqrtD = std::sqrt(discriminant);
            float t0 = (-b - sqrtD) / (2.0f * a);
            float t1 = (-b + sqrtD) / (2.0f * a);

            // If both t0 and t1 are negative, no intersection in front of ray
            if (t0 < 0.0f && t1 < 0.0f)
                return false;

            // Get nearest positive intersection
            t = (t0 > 0.0f) ? t0 : t1;
            return true;
        }
};

#endif
