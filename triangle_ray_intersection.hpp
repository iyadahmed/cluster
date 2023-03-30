#pragma once

#include <glm/glm.hpp>

bool ray_triangle_intersect(const glm::vec3 &orig, const glm::vec3 &dir,
                            const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2,
                            float &t) {
    // Compute the edges of the triangle
    glm::vec3 e1 = v1 - v0;
    glm::vec3 e2 = v2 - v0;

    // Compute the determinant
    glm::vec3 pvec = glm::cross(dir, e2);
    float det = glm::dot(e1, pvec);

    // Check if the ray is parallel to the triangle
    if (det < 1e-8 && det > -1e-8) {
        return false;
    }

    float inv_det = 1 / det;

    // Compute the u parameter
    glm::vec3 tvec = orig - v0;
    float u = glm::dot(tvec, pvec) * inv_det;
    if (u < 0 || u > 1) {
        return false;
    }

    // Compute the v parameter
    glm::vec3 qvec = glm::cross(tvec, e1);
    float v = glm::dot(dir, qvec) * inv_det;
    if (v < 0 || u + v > 1) {
        return false;
    }

    // Compute the t parameter
    t = glm::dot(e2, qvec) * inv_det;

    return true;
}