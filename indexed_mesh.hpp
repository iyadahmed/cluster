#pragma once

#include <vector>
#include <iostream>
#include <unordered_map>

#include <glm/vec3.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <tiny_stl.hpp>

struct IndexedMesh {
    std::vector<glm::vec3> unique_vertices;
    std::vector<size_t> indices;
};

IndexedMesh read_indexed_mesh_from_stl(const char *filepath) {
    std::cout << "Attempting to read STL mesh file from " << filepath << std::endl;

    auto reader = Tiny_STL::create_reader(filepath);

    struct GLMVec3Hash {
        std::size_t operator()(const glm::vec3 &v) const {
            return std::hash<float>{}(v.x) ^ std::hash<float>{}(v.y) ^ std::hash<float>{}(v.z);
        }
    };

    std::unordered_map<glm::vec3, size_t, GLMVec3Hash> vertex_map;
    std::vector<glm::vec3> unique_vertices;
    std::vector<size_t> indices;
    size_t num_successfully_read_triangles = 0;
    size_t vi = 0;
    Tiny_STL::Triangle t;
    while (reader->read_next_triangle(&t)) {
        for (auto &vertex: t.vertices) {
            glm::vec3 v(vertex[0], vertex[1], vertex[2]);
            auto it = vertex_map.find(v);
            if (it == vertex_map.end()) {
                // Vertex does not exist in map
                unique_vertices.push_back(v);
                vertex_map[v] = vi;
                indices.push_back(vi);
                vi++;
            } else {
                indices.push_back(it->second);
            }
        }
        num_successfully_read_triangles++;
    }
    std::cout << "Successfully read " << num_successfully_read_triangles << " triangles" << std::endl;
    std::cout << "Number of unique vertices = " << unique_vertices.size() << std::endl;

    return IndexedMesh{unique_vertices, indices};
}
