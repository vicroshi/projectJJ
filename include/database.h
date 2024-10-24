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
float* read_fvecs(std::string* fname, size_t* dim, size_t* vecnum);
int* read_ivecs(std::string* fname, size_t* dim, size_t* vecnum);
template <typename T>
struct Matrix{
    size_t dim;
    size_t vecnum;
    std::span<T> vecs;
    Matrix(size_t dim, size_t vecnum, T* data):dim(dim), vecnum(vecnum), vecs(data, vecnum*dim) {}
    ~Matrix() {
        munmap(vecs.data(), vecnum*dim*sizeof(T));
//        delete[] vecs;
    }
    T get(int row, int col){
        return vecs[row*dim+col];
    }
    double sq_euclid(int row1, int row2){
        double dist = 0;
        double diff;
        auto vec1 = vecs[row1*dim];
        auto vec2 = vecs[row2*dim];
        for (int i = 0; i < dim; i++) {
            diff = vec1[i] - vec2[i];
            dist += diff*diff;
        }
        return dist;
    }

     int medoid_naive() {
        double dist;
        double min_dist = std::numeric_limits<double>::max();
        double medoid_idx = 0;
        for (int i = 0; i < vecnum; i++) {
            dist = 0;
            for (int j = 0; j < vecnum; j++) {
                if (i != j) {
                    dist += sq_euclid(i, j);
                }
            }
            if (dist < min_dist) {
                min_dist = dist;
                medoid_idx = i;
            }
        }
    }

};
#endif //DATABASE_H
