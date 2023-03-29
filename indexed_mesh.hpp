#pragma once

#include <vector>

#include <glm/vec3.hpp>

struct IndexedMesh {
    std::vector<glm::vec3> unique_vertices;
    std::vector<size_t> indices;
};

IndexedMesh read_indexed_mesh_from_stl(const char *filepath);
