#include <array>
#include <algorithm>
#include <cstdio>
#include <vector>
#include <iostream>

#include "hash.h"

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

//@param in data to hash
//@returns 32-bit hash of in
static uint32_t hash(float3 in) {
    uint32_t h = 0;
    h = int_hash32(h ^ __builtin_bit_cast(uint32_t, in[0]));
    h = int_hash32(h ^ __builtin_bit_cast(uint32_t, in[1]));
    h = int_hash32(h ^ __builtin_bit_cast(uint32_t, in[2]));
    return h;
}

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
    STLBinaryTriangle t{};
    while (1 == fread(&t, sizeof(STLBinaryTriangle), 1, file)) {
        for (auto v: t.vertices) {
            vertices.push_back(v);
        }
    }

    std::sort(vertices.begin(), vertices.end());
    auto it = std::unique(vertices.begin(), vertices.end());
    std::cout << "Number of unique vertices = " << std::distance(vertices.begin(), it) << std::endl;

    return EXIT_SUCCESS;
}
