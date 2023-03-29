#include <cstdio>
#include <iostream>
#include <filesystem>

#include <glm/common.hpp>

#include "indexed_mesh.hpp"


static int new_node_index = 0;

struct Node {
    glm::vec3 upper, lower;
    int left, right;
};


static int new_node() {
    return new_node_index++;
}

static int build_tree(std::vector<glm::vec3> &vertices, int start, int last, Node *nodes) {
    int node_index = new_node_index++;
    Node *node = nodes + node_index;

    node->upper = node->lower = vertices[start];
    for (int i = start; i <= last; i++) {
        node->upper = glm::max(node->upper, vertices[i]);
        node->lower = glm::min(node->lower, vertices[i]);
    }

    if (start == last) {
        node->left = node->right = start;
    } else {
        int mid = start / 2 + last / 2;

        node->left = build_tree(vertices, start, mid, nodes);
        node->right = build_tree(vertices, mid + 1, last, nodes);
    }

    return node_index;
}

int count_leaf_nodes(int node_index, Node *nodes) {
    Node *node = nodes + node_index;
    if (node == nullptr) {
        return 0;
    }
    if (node->left == node->right) {
        return 1;
    }

    return count_leaf_nodes(node->left, nodes) + count_leaf_nodes(node->right, nodes);
}

bool contains_point(Node *nodes, int node_index, const glm::vec3 &point, const std::vector<glm::vec3> &vertices) {
    Node *node = nodes + node_index;
    if (node->left == node->right) {
        return vertices[node->left] == point;
    }
    return contains_point(nodes, node->left, point, vertices) || contains_point(nodes, node->right, point, vertices);
}

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Expected arguments: path/to/stl/or/directory/of/STLs\n");
        return 0;
    }
    char *input_path = argv[1];

    IndexedMesh mesh;

    if (std::filesystem::is_directory(input_path)) {
        for (const auto &entry: std::filesystem::directory_iterator(input_path)) {
            const std::filesystem::path &path = entry.path();
            if (path.extension() == ".stl") {
                mesh = read_indexed_mesh_from_stl((const char *) path.string().c_str());
            }
        }
    } else {
        mesh = read_indexed_mesh_from_stl(input_path);
    }

    Node *nodes = new Node[2 * mesh.unique_vertices.size()];
    int root_index = build_tree(mesh.unique_vertices, 0, mesh.unique_vertices.size() - 1, nodes);
    std::cout << count_leaf_nodes(root_index, nodes) << std::endl;

    std::cout << contains_point(nodes, 0, mesh.unique_vertices[0], mesh.unique_vertices) << std::endl;

    return EXIT_SUCCESS;
}