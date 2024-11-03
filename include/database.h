//
// Created by vic on 21/10/2024.
//

#ifndef DATABASE_H
#define DATABASE_H
#include <iostream>
#include <unistd.h>
#include <string>
#include <sys/mman.h>
#include <sys/stat.h>
#include <cstring>
#include <fcntl.h>
#include <memory>
#include <span>
#include <limits>
#include <immintrin.h>
#include <cstddef>
#include <set>

template <typename T>
struct Matrix{
    size_t dim;
    size_t vecnum;
    std::span<T> vecs;
    
    Matrix(size_t dim, size_t vecnum, T* data):dim(dim), vecnum(vecnum), vecs(data, vecnum*dim) {}

    T get(int row, int col){
        return vecs[row*dim+col];
    }

    std::span<T> row(int row){
        return vecs.subspan(row*dim, dim);
    }


    static constexpr  double sq_euclid(const std::span<T>& row1,const std::span<T>& row2,const size_t& dim){
        // Using AVX for float types
        if constexpr(std::is_floating_point_v<T>){
            __m256 sum = _mm256_setzero_ps(); // Initialize sum to 0
            size_t i = 0;
            // Process 8 floats at a time
            for (; i + 8 <= dim; i += 8) {
                __m256 a_vec = _mm256_loadu_ps(row1.data() + i);
                __m256 b_vec = _mm256_loadu_ps(row2.data() + i);
                __m256 diff = _mm256_sub_ps(a_vec, b_vec);
                sum = _mm256_fmadd_ps(diff, diff, sum); // sum += diff * diff
            }
            // Sum the results
            float temp[8];
            _mm256_storeu_ps(temp, sum);
            double result = temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5] + temp[6] + temp[7];
            // Process any remaining elements
            for (; i < dim; ++i) {
                float diff = row1.data()[i] - row2.data()[i];
                result += diff * diff;
            }
            return result;
        //for ints
        } else if constexpr (std::is_integral_v<T>) {
            __m256 sum = _mm256_setzero_ps(); // Initialize sum to 0
            size_t i = 0;
            // Process 8 integers at a time
            for (; i + 8 <= dim; i += 8) {
                __m256i a_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row1.data() + i)); // Load 8 ints
                __m256i b_vec = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(row2.data() + i)); // Load 8 ints
                __m256i diff = _mm256_sub_epi32(a_vec, b_vec); // Calculate difference
                // Convert the integer difference to float for squared distance calculation
                __m256 diff_f = _mm256_cvtepi32_ps(diff);
                sum = _mm256_fmadd_ps(diff_f, diff_f, sum); // sum += diff_f * diff_f
            }
            // Sum the results
            float temp[8];
            _mm256_storeu_ps(temp, sum);
            double result = temp[0] + temp[1] + temp[2] + temp[3] + temp[4] + temp[5] + temp[6] + temp[7];
            // Process any remaining elements
            for (; i < dim; ++i) {
                int diff =row1.data()[i] - row2.data()[i];
                result += static_cast<double>(diff * diff);
            }
            return result;
        }
    }
    

    // static double sq_euclid(std::span<T> row1, std::span<T> row2,const size_t size){
    //     double dist = 0;
    //     double diff;
    //     // auto vec1 = vecs[row1*dim];
    //     // auto vec2 = vecs[row2*dim];
    //     for (size_t i = 0; i < size; i++) {
    //         diff = row1[i] - row2[i];
    //         dist += diff*diff;
    //     }
    //     return dist;
    // }

    int medoid_naive() {
        double dist;
        double min_dist = std::numeric_limits<double>::max();
        int medoid_idx = 0;
        for (size_t i = 0; i < vecnum; i++) {
            dist = 0;
            for (size_t j = 0; j < vecnum; j++) {
                if (i != j) {
                    dist += sq_euclid(row(i),row(j),row(i).size());
                    // dist += sq_euclid(get_row(i), get_row(j));
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
