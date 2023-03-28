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

IndexedMesh read_indexed_mesh_from_stl(const char *filepath);
