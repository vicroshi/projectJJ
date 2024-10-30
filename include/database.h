//
// Created by vic on 21/10/2024.
//

#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <span>
#include <limits>
#include <immintrin.h>
#include <cstddef>


template <typename T>
struct Matrix{
    size_t dim;
    size_t vecnum;
    std::span<T> vecs;
    Matrix(size_t dim, size_t vecnum, T* data):dim(dim), vecnum(vecnum), vecs(data, vecnum*dim) {}
    T get(int row, int col){
        return vecs[row*dim+col];
    }
        std::span<T> get_row(int row){
        return vecs.subspan(row*dim, dim);
    }
//    static double sq_euclid(std::span<T> row1, std::span<T> row2){
//        double dist = 0;
//        double diff;
//        // auto vec1 = vecs[row1*dim];
//        // auto vec2 = vecs[row2*dim];
//        for (size_t i = 0; i < row1.size(); i++) {
//            diff = row1[i] - row2[i];
//            dist += diff*diff;
//        }
//        return dist;
//    }

    static constexpr  double sq_euclid(const std::span<T>& row1,const std::span<T>& row2) {
        __m256 sum = _mm256_setzero_ps(); // Initialize sum to 0
        unsigned i;
        size_t dim = 128;
        auto a = reinterpret_cast<float*>(row1.data());
        auto b = reinterpret_cast<float*>(row2.data());
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

    int medoid_naive() {
        double dist;
        double min_dist = std::numeric_limits<double>::max();
        int medoid_idx = 0;
        for (size_t i = 0; i < vecnum; i++) {
            dist = 0;
            for (size_t j = 0; j < vecnum; j++) {
                if (i != j) {
                    dist += sq_euclid(get_row(i), get_row(j));
                }
            }
            if (dist < min_dist) {
                min_dist = dist;
                medoid_idx = i;
            }
        }
        return medoid_idx;
    }
};


#endif //DATABASE_H
