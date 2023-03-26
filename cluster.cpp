#include <array>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <iostream>

using float3 = std::array<float, 3>;

// pragma-packed in case you wanted to read an array of these from file
#pragma pack(push, 1)
struct STLBinaryTriangle {
    float3 normal;
    float3 vertices[3];
    uint16_t attribute_byte_count;
};
#pragma pack(pop)
static_assert(sizeof(STLBinaryTriangle) == sizeof(float[3]) + sizeof(float[3][3]) + sizeof(uint16_t));

int main(int argc, char **argv) {
    if (argc != 2) {
        printf("Expected arguments: mesh.stl\n");
        return 0;
    }
    char *filepath = argv[1];
    printf("Reading mesh from: %s\n", filepath);

    FILE *file = fopen(filepath, "rb");
    if (file == nullptr) {
        printf("Failed to open file\n");
        return 1;
    }

    fseek(file, 80, SEEK_SET); // Skip binary header
    uint32_t num_reported_tris;
    fread(&num_reported_tris, sizeof(uint32_t), 1, file);
    printf("Number of triangles = %u\n", num_reported_tris);

    if (num_reported_tris == 0) {
        printf("Empty mesh\n");
        return EXIT_SUCCESS;
    }

    std::vector<float3> vertices;
    std::vector<size_t> indices;
    size_t i = 0;
    STLBinaryTriangle t{};
    while (1 == fread(&t, sizeof(STLBinaryTriangle), 1, file)) {
        for (auto v: t.vertices) {
            vertices.push_back(v);
            indices.push_back(i++);
        }
    }

    std::sort(indices.begin(), indices.end(), [&](size_t ai, size_t bi) {
        return vertices[ai] < vertices[bi];
    });
    size_t current_unique_vertex_index = indices[0];
    for (unsigned long long &index: indices) {
        if (vertices[index] == vertices[current_unique_vertex_index]) {
            index = current_unique_vertex_index;
        } else {
            current_unique_vertex_index = index;
        }
    }
    auto it = std::unique(indices.begin(), indices.end());
    std::cout << "Number of unique vertices = " << std::distance(indices.begin(), it) << std::endl;

    return EXIT_SUCCESS;
}
