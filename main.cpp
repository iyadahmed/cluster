#include <cstdio>
#include <iostream>
#include <filesystem>

#include "indexed_mesh.hpp"

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Expected arguments: path/to/folder/full/of/STLs\n");
        return 0;
    }
    char *folder_path = argv[1];

    for (const auto &entry: std::filesystem::directory_iterator(folder_path)) {
        auto path = entry.path();
        if (path.extension() == ".stl") {
            auto mesh = read_indexed_mesh_from_stl((const char *) path.string().c_str());
        }
    }

    return EXIT_SUCCESS;
}