#ifndef CLUSTER_INDEXED_TRI_MESH_HPP
#define CLUSTER_INDEXED_TRI_MESH_HPP

#include <array>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <iostream>
#include <optional>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Vertex {
    glm::vec3 pos;
    size_t original_index;
};

struct IndexedMesh {
    std::vector<glm::vec3> unique_vertices;
    std::vector<size_t> indices;
};


std::optional<std::array<glm::vec3, 3>> read_stl_binary_triangle(FILE *file) {
    // Skip normal
    if (fseek(file, sizeof(glm::vec3), SEEK_CUR) != 0) {
        return std::nullopt;
    }
    std::array<glm::vec3, 3> vertices{};
    if (fread(vertices.data(), sizeof(glm::vec3[3]), 1, file) != 1) {
        return std::nullopt;
    }
    // Skip "attribute_byte_count"
    if (fseek(file, sizeof(uint16_t), SEEK_CUR) != 0) {
        return std::nullopt;
    }
    return vertices;
}

std::optional<IndexedMesh> read_indexed_mesh_from_stl(const char *filepath) {
    std::cout << "Attempting to read binary STL mesh file from " << filepath << std::endl;

    FILE *file = fopen(filepath, "rb");
    if (file == nullptr) {
        std::cerr << "Failed to open file" << std::endl;
        return std::nullopt;
    }

    if (fseek(file, 80, SEEK_SET) != 0) {
        std::cerr << "Failed to skip binary header" << std::endl;
        return std::nullopt;
    }

    uint32_t num_reported_tris = 0;
    if (fread(&num_reported_tris, sizeof(uint32_t), 1, file) != 1) {
        std::cerr << "Failed to read number of triangles" << std::endl;
        return std::nullopt;
    }

    std::cout << "Number of triangles reported by file = " << num_reported_tris << std::endl;

    if (num_reported_tris == 0) {
        std::cerr << "Empty mesh" << std::endl;
        return std::nullopt;
    }

    // Read vertices
    std::vector<Vertex> vertices;
    {
        size_t i = 0;
        while (auto t = read_stl_binary_triangle(file)) {
            for (glm::vec3 v: t.value()) {
                vertices.push_back({v, i});
                i++;
            }
        }
        if (i != (num_reported_tris * 3)) {
            std::cerr
                    << "Warning: number of successfully read triangles does not match the number of triangles reported by file"
                    << std::endl;
        }
        std::cout << "Number of successfully read triangles = " << (i / 3) << std::endl;
    }

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

#ifndef NDEBUG
    // Sanity check
    std::vector<size_t> indices_copy = indices;
    std::sort(indices_copy.begin(), indices_copy.end());
    auto it = std::unique(indices_copy.begin(), indices_copy.end());
    assert(std::distance(indices_copy.begin(), it) == unique_vertices.size());
#endif

    return IndexedMesh{unique_vertices, indices};
}

#endif //CLUSTER_INDEXED_TRI_MESH_HPP
