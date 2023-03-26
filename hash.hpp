#pragma once

#include <cstdint>
#include <array>

#ifndef __cplusplus
#define __builtin_bit_cast(type, arg) ({ \
    (union{                              \
        __typeof(({arg;})) in;           \
        type out;                        \
    }){({arg;})}.out;                    \
})
#endif

// least-biased 32-bit hash
// magic numbers and shifts from hash prospector
static inline uint32_t int_hash32(uint32_t x){
    x ^= x >> 17;
    x *= 0xed5ad4bbU;
    x ^= x >> 11;
    x *= 0xac4c1b51U;
    x ^= x >> 15;
    x *= 0x31848babU;
    x ^= x >> 14;
    return x;
}

//@param in data to hash
//@returns 32-bit hash of in
static uint32_t hash(std::array<float, 3> in) {
    uint32_t h = 0;
    h = int_hash32(h ^ __builtin_bit_cast(uint32_t, in[0]));
    h = int_hash32(h ^ __builtin_bit_cast(uint32_t, in[1]));
    h = int_hash32(h ^ __builtin_bit_cast(uint32_t, in[2]));
    return h;
}