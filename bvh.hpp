#pragma once

#include <vector>

#include <glm/vec3.hpp>
#include <glm/common.hpp>

class BVH {
public:
    explicit BVH(const std::vector<glm::vec3> &input_vertices) {
        vertices = input_vertices;
        nodes.resize(2 * vertices.size());
        build_tree(0, vertices.size() - 1);

        assert(count_leaf_nodes(0) == vertices.size());
    }

    int count_leaf_nodes() {
        return count_leaf_nodes(0);
    }

    bool contains_point(glm::vec3 point) {
        return contains_point(0, point);
    }

private:
    struct Node {
        glm::vec3 upper, lower;
        int left, right;
    };

    std::vector<Node> nodes;
    std::vector<glm::vec3> vertices;
    int new_node_index = 0;

    int build_tree(int start, int last) {
        int node_index = new_node_index++;

        Node &node = nodes[node_index];

        node.upper = node.lower = vertices[start];
        for (int i = start; i <= last; i++) {
            node.upper = glm::max(node.upper, vertices[i]);
            node.lower = glm::min(node.lower, vertices[i]);
        }

        if (start == last) {
            node.left = node.right = start;
        } else {
            // TODO: partition vertices
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
            return vertices[node.left] == point;
        }

        return contains_point(node.left, point) || contains_point(node.right, point);
    }
};