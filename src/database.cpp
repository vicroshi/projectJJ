//
// Created by vic on 21/10/2024.
//
#include "database.h"
float* read_fvecs(std::string* name, size_t* dim, size_t* vecnum) {
    int fd = open(name->c_str(), O_RDONLY);
    if (fd == -1) {
        perror("Error opening file for reading");
        return nullptr;
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        perror("Error getting the file size");
        return nullptr;
    }
    read(fd, dim, sizeof(int));
    float* p = (float*)mmap(nullptr, sb.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) {
        perror("Error mapping the file");
        return nullptr;
    }
    close(fd);
    int d = *reinterpret_cast<int *>(p);
    *dim = d;
    *vecnum = sb.st_size / ((d + 1) * 4);
//    float* x = new float[*vecnum * d];
    for (size_t i = 0; i < *vecnum; i++) {
        std::memmove(p + i * d, p + 1 + i * (d + 1), d * sizeof(*p));
    }
//    munmap(p, sb.st_size);
    return p;
}

int* read_ivecs(std::string* name, size_t* dim, size_t* vecnum) {
    return (int*) read_fvecs(name, dim, vecnum);
}