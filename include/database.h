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
    static double sq_euclid(std::span<T> row1, std::span<T> row2){
        double dist = 0;
        double diff;
        // auto vec1 = vecs[row1*dim];
        // auto vec2 = vecs[row2*dim];
        for (size_t i = 0; i < row1.size(); i++) {
            diff = row1[i] - row2[i];
            dist += diff*diff;
        }
        return dist;
    }

    int medoid_naive() {
        double dist;
        double min_dist = std::numeric_limits<double>::max();
        int medoid_idx = 0;
        for (int i = 0; i < vecnum; i++) {
            dist = 0;
            for (int j = 0; j < vecnum; j++) {
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
