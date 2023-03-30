#pragma once

#include <utility>

#include <glm/glm.hpp>

bool ray_aabb_intersect(const glm::vec3 &orig, const glm::vec3 &dir,
                        const glm::vec3 &min, const glm::vec3 &max,
                        float &tmin, float &tmax) {
    tmin = 0;
    tmax = std::numeric_limits<float>::max();

    for (int i = 0; i < 3; i++) {
        if (std::abs(dir[i]) < 1e-8) {
            // Ray is parallel to the slab
            if (orig[i] < min[i] || orig[i] > max[i]) {
                return false;
            }
        } else {
            // Compute the intersection distance of the planes
            float ood = 1.0f / dir[i];
            float t1 = (min[i] - orig[i]) * ood;
            float t2 = (max[i] - orig[i]) * ood;

            // Make t1 be the intersection with the near plane
            if (t1 > t2) {
                std::swap(t1, t2);
            }

            // Compute the intersection of the slab intersection intervals
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);

            // Exit with no collision as soon as slab intersection becomes empty
            if (tmin > tmax) {
                return false;
            }
        }
    }

    return true;
}