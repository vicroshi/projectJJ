//
// Created by vic on 29/10/2024.
//

#ifndef DISTANCE_H
#define DISTANCE_H
#include <immintrin.h>
#include <cstddef>
inline float sq_euclid(float *a, float *b, size_t dim){
    __m256 sum = _mm256_setzero_ps(); // Initialize sum to 0
    unsigned i;
    // dim = 104;
    for (i = 0; i + 7 < dim; i += 4)
    {                                              // Process 8 floats at a time
        __m256 a_vec = _mm256_loadu_ps(&a[i]);      // Load 8 floats from a
        __m256 b_vec = _mm256_loadu_ps(&b[i]);      // Load 8 floats from b
        __m256 diff = _mm256_sub_ps(a_vec, b_vec); // Calculate difference
        sum = _mm256_fmadd_ps(diff, diff, sum);    // Calculate sum of squares
    }
    float result = 0;
    // float temp[8] __attribute__((aligned(32)));
    // _mm256_store_ps(temp, sum);
    for (unsigned j = 0; j < 8; ++j)
    { // Reduce sum to a single float
        result += ((float *)&sum)[j];
    }
    // for (; i < dim; ++i) { // Process remaining floats
    //     float diff = a[i] - b[i];
    //     result += diff * diff;
    // }
    return result; // Return square root of sum
}
#endif //DISTANCE_H
