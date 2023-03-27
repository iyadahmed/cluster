#include <cstdio>
#include <iostream>

#include "indexed_mesh.hpp"


int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Expected arguments: mesh.stl\n");
        return 0;
    }
    char *filepath = argv[1];
    auto mesh = read_indexed_mesh_from_stl(filepath);
    if (!mesh.has_value()) {
        std::cout << "Failed to read mesh" << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}