#pragma once

#include <vector>
#include <numeric>

#include <glm/vec3.hpp>
#include <glm/common.hpp>

#include "indexed_mesh.hpp"
#include "aabb_ray_intersection.hpp"
#include "triangle_ray_intersection.hpp"

class BVH {
public:
    explicit BVH(const IndexedMesh &input_mesh) {
        mesh = input_mesh;
        triangles.resize(mesh.indices.size() / 3);
        std::iota(triangles.begin(), triangles.end(), 0);

        nodes.resize(2 * triangles.size());
        build_tree(0, triangles.size() - 1);
        assert(count_leaf_nodes(0) == triangles.size());
    }

    int count_leaf_nodes() {
        return count_leaf_nodes(0);
    }

    bool contains_point(glm::vec3 point) {
        return contains_point(0, point);
    }

    bool intersect_ray(glm::vec3 origin, glm::vec3 direction, float &t) {
        return intersect_ray(0, origin, direction, t);
    }

private:
    struct Node {
        glm::vec3 upper, lower;
        int left, right;
    };

    std::vector<Node> nodes;
    IndexedMesh mesh;
    std::vector<size_t> triangles;
    int new_node_index = 0;

    int build_tree(int start, int last) {
        int node_index = new_node_index++;

        Node &node = nodes[node_index];

        node.upper = node.lower = mesh.unique_vertices[mesh.indices[triangles[start] * 3]];

        for (int i = start; i <= last; i++) {
            for (int vi = 0; vi < 3; vi++) {
                glm::vec3 v = mesh.unique_vertices[mesh.indices[triangles[i] * 3 + vi]];
                node.upper = glm::max(node.upper, v);
                node.lower = glm::min(node.lower, v);
            }
        }

        if (start == last) {
            node.left = node.right = start;
        } else {
            // TODO: partition triangles
            int mid = start / 2 + last / 2;
            node.left = build_tree(start, mid);
            node.right = build_tree(mid + 1, last);
        }

        return node_index;
    }

    int count_leaf_nodes(int node_index) {
        Node node = nodes[node_index];
        if (node.left == node.right) {
            return 1;
        }

        return count_leaf_nodes(node.left) + count_leaf_nodes(node.right);
    }

    bool contains_point(int node_index, glm::vec3 point) {
        Node node = nodes[node_index];
        if (node.left == node.right) {
            for (int vi = 0; vi < 3; vi++) {
                glm::vec3 v = mesh.unique_vertices[mesh.indices[triangles[node.left] * 3 + vi]];
                if (v == point) {
                    return true;
                }
            }
            return false;
        }

        return contains_point(node.left, point) || contains_point(node.right, point);
    }

    bool intersect_ray(int node_index, glm::vec3 origin, glm::vec3 direction, float &t) {
        Node &node = nodes[node_index];
        float tmin, tmax;
        if (!ray_aabb_intersect(origin, direction, node.lower, node.upper, tmin, tmax)) {
            return false;
        }

        if (node.left == node.right) {
            glm::vec3 v1 = mesh.unique_vertices[mesh.indices[triangles[node.left] * 3 + 0]];
            glm::vec3 v2 = mesh.unique_vertices[mesh.indices[triangles[node.left] * 3 + 1]];
            glm::vec3 v3 = mesh.unique_vertices[mesh.indices[triangles[node.left] * 3 + 2]];
            float local_t;
            if (ray_triangle_intersect(origin, direction, v1, v2, v3, local_t)) {
                t = std::min(t, local_t);
                return true;
            }
            return false;
        }

        return intersect_ray(node.left, origin, direction, t) || intersect_ray(node.right, origin, direction, t);
    }
};