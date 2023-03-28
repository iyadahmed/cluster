#ifndef CLUSTER_INDEXED_TRI_MESH_HPP
#define CLUSTER_INDEXED_TRI_MESH_HPP

#include <array>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <iostream>
#include <optional>
#include <memory>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_stl.hpp>


struct Vertex {
    glm::vec3 pos;
    size_t original_index;
};

struct IndexedMesh {
    std::vector<glm::vec3> unique_vertices;
    std::vector<size_t> indices;
};

std::optional<IndexedMesh> read_indexed_mesh_from_stl(const char *filepath) {
    std::cout << "Attempting to read binary STL mesh file from " << filepath << std::endl;

    auto reader = Tiny_STL::create_reader(filepath);

    // Read vertices
    std::vector<Vertex> vertices;
    {
        size_t vi = 0;
        Tiny_STL::Triangle t;
        while (reader->read_next_triangle(&t)) {
            for (auto &vertex: t.vertices) {
                vertices.push_back({{vertex[0], vertex[1], vertex[2]}, vi});
                vi++;
            }
        }
        std::cout << "Successfully read " << vi << " vertices" << " (" << (vi / 3) << " triangles)" << std::endl;
        if (vi == 0) {
            std::cerr << "Empty mesh" << std::endl;
            return std::nullopt;
        }
    }

#ifndef NDEBUG
    // Make a copy of vertices before sorting for testing purposes
    std::vector<Vertex> vertices_copy = vertices;
#endif

    // Sort vertices lexicographically by their 3D position
    std::sort(vertices.begin(), vertices.end(), [](const Vertex &a, const Vertex &b) {
        const float *v1 = glm::value_ptr(a.pos);
        const float *v2 = glm::value_ptr(b.pos);
        return std::lexicographical_compare(v1, v1 + 3, v2, v2 + 3);
    });

    // Build indexed mesh
    std::vector<glm::vec3> unique_vertices;
    unique_vertices.push_back(vertices[0].pos);
    std::vector<size_t> indices(vertices.size());
    indices[vertices[0].original_index] = 0;
    {
        size_t j = 0;
        size_t last_unique_vertex_index = 0;
        for (size_t i = 0; i < vertices.size(); i++) {
            if (vertices[i].pos != vertices[j].pos) {
                j = i;
                unique_vertices.push_back(vertices[i].pos);
                last_unique_vertex_index++;
            }

            indices[vertices[i].original_index] = last_unique_vertex_index;
        }
    }

    std::cout << "Number of unique vertices = " << unique_vertices.size() << std::endl;

#ifndef NDEBUG
    // TODO: use a testing framework
    // Sanity checks
    std::cerr << "Warning: debug build, running sanity checks" << std::endl;

    // 1. Number of unique indices == number of unique vertices
    {
        std::vector<size_t> indices_copy = indices;
        std::sort(indices_copy.begin(), indices_copy.end());
        auto it = std::unique(indices_copy.begin(), indices_copy.end());
        assert(std::distance(indices_copy.begin(), it) == unique_vertices.size());
    }

    // 2. Number of unique vertices == number of unique vertices obtained from a set with a hash function for floats
    // TODO

    // 3. Rebuilding non-indexed mesh matches original
    {
        for (size_t i = 0; i < indices.size(); i++) {
            assert(unique_vertices[indices[i]] == vertices_copy[i].pos);
        }
    }
#endif

    return IndexedMesh{unique_vertices, indices};
}

#endif //CLUSTER_INDEXED_TRI_MESH_HPP
