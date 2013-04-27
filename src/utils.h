#ifndef UTILS_H
#define UTILS_H
#include <memory>
#include <vector>
#include <stdint.h>
#include <unordered_map>
#include "MurmurHash3.h"
#include <random>
#include <fstream>
#include <string>
#include <iostream>
#include <sstream>
#include <stdexcept>

/* Courtesy of Kirill V Lyadvinsky. Though not used yet.
 *template<int> void Murmur3Hasher
 *(const void * key, int len, uint32_t seed, void* out);
 *
 *    template<> void Murmur3Hasher<4>
 *(const void * key, int len, uint32_t seed, void* out)
 *{
 *    MurmurHash3_x86_128 ( key, len, seed, out);
 *};
 *
 *    template<> void Murmur3Hasher<8>
 *(const void * key, int len, uint32_t seed, void* out)
 */
// helper function just to hide clumsy syntax
#define FGCVERSION 0.2
inline void Murmur3
    (const void * key, int len, uint32_t seed, void* out)
{
    MurmurHash3_x64_128 ( key, len, seed, out);
    /*
     *Murmur3Hasher<sizeof(size_t)>
     *( key, len,  seed, void* out);
     */
};

#endif
