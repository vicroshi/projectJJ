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
#include <unordered_set>
#include <vector>
#include <unordered_map>
#include <random>
#include <algorithm>
#include <omp.h>


template <typename T>
struct Matrix{
    private:
        static const std::vector<T> empty_vector; // Default empty vector
    
    public:
    size_t dim;
    size_t vecnum;
    std::span<T> vecs;
    const std::vector<T>& vec_filter; //filter for each vec
    const std::vector<T>& filters_set; //set of all filters
    
    

    Matrix(): dim(0), vecnum(0), vecs({}), vec_filter(empty_vector), filters_set(empty_vector){}
    // Static default instance
    static const Matrix<T>& default_instance() {
        static Matrix<T> default_matrix;
        return default_matrix;
    }
    //project 1 constructor
    Matrix(size_t dim, size_t vecnum, T* data): dim(dim), vecnum(vecnum), vecs(data, vecnum * dim), vec_filter(empty_vector), filters_set(empty_vector){}
    //project 2 constructor
    Matrix(size_t dim, size_t vecnum,std::vector<T>* data , const std::vector<T>& vec_filter,  const std::vector<T>& filters_set):
            dim(dim), vecnum(vecnum), vecs(data->data(),vecnum*dim), vec_filter(vec_filter), filters_set(filters_set){}
    
    T get(int row, int col) const {
        return vecs[row*dim+col];
    }

     std::span<T> row(int row) const{
        return vecs.subspan(row*dim, dim);
    }

    //using SIMD instructions to calculate squared distance faster, AVX/AVX2 required
    static double sq_euclid(const std::span<T>& row1,const std::span<T>& row2,const size_t& dim){
        //for floats
        if constexpr(std::is_floating_point_v<T>){
            __m256 sum = _mm256_setzero_ps(); //Initialize sum to 0
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
    
    //naive approach, not used
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

    int medoid_rand(const std::vector<int>& Pf = {}) const {
        int medoid_idx = 0;
        std::vector<int> vec;
        if (Pf.empty()) {
            vec.resize(vecnum);
            std::iota(vec.begin(), vec.end(), 0);
        } else {
            vec = Pf;
        }
        std::random_device rd;
        std::mt19937 g(rd());
        std::ranges::sample(vec, &medoid_idx, 1, g);
        return medoid_idx;
    }

    int medoid_naive(const std::vector<int>& Pf = {}) const {
        double global_min_dist = std::numeric_limits<double>::max();
        int global_medoid_idx = 0;

        std::vector<int> vec;
        if (Pf.empty()) {
            vec.resize(vecnum);
            std::iota(vec.begin(), vec.end(), 0);
        } else {
            vec = Pf;
        }

        #pragma omp parallel 
        {
            // std::cout << "OpenMP is running medoid_naive with " << omp_get_num_threads() << " thread(s)." << std::endl;

            double local_min_dist = std::numeric_limits<double>::max();
            int local_medoid_idx = 0;

            #pragma omp for  nowait
            for (size_t idx = 0; idx < vec.size(); idx++) {
                auto i = vec[idx];
                double dist = 0;

                for (auto j : vec) {
                    if (i != j) {
                        dist += sq_euclid(row(i), row(j), dim);
                    }
                }

                if (dist < local_min_dist) {
                    local_min_dist = dist;
                    local_medoid_idx = i;
                }
            }

            #pragma omp critical
            {
                if (local_min_dist < global_min_dist) {
                    global_min_dist = local_min_dist;
                    global_medoid_idx = local_medoid_idx;
                }
            }
        }

    return global_medoid_idx;
}

                                                        //M keeps the index of the starting point for each filter
    void find_medoid(const size_t& t,std::unordered_map<T,int>& M,std::unordered_map<T, std::vector<int>>& Pf){
        std::random_device rd;                                                                                  //Pf keeps points' index for each filter
        std::mt19937 g(rd());
        size_t tau; //if t > no. of points with a specific filter, i have to use the no. of filters, so tau= t>no. of points ? no. of points : t 
        std::unordered_map<int, int>T_counter; //T is a counter for each point
        for(size_t i=0;i<vecnum;i++){
            //fill hashmap
            // Pf[(vec_filter)[i]].push_back(i);
            T_counter[i]=0; //initialize counter to 0 for all points
        }
        
        // for each filter, i shuffle the hash map vectors and i take the first tau
        for(auto& f:(filters_set)){
            size_t no_of_points=Pf[f].size();
            tau= t>no_of_points ? no_of_points : t;
            std::shuffle(Pf[f].begin(),Pf[f].end(),g); //shuffle the whole vector

            //find the min,looking upto tau
            int min = std::numeric_limits<int>::max(),p_star_idx=-1;
            for(size_t j=0;j<tau;j++){
                if(T_counter[Pf[f][j]] < min){
                    min=T_counter[Pf[f][j]];

                    p_star_idx=Pf[f][j];
                }
            }

            //update M and T_counter
            M[f]=p_star_idx;
            T_counter[p_star_idx]++;

        }

    }



};

template <typename T>
const std::vector<T> Matrix<T>::empty_vector = {};

#endif //DATABASE_H
