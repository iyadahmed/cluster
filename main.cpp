#include <cstdio>
#include <iostream>
#include <filesystem>

#include "indexed_mesh.hpp"
#include "bvh.hpp"


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

    BVH bvh(mesh);
    std::cout << bvh.contains_point(mesh.unique_vertices[0]) << std::endl;

    return 0;
}