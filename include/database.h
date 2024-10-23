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
float* read_fvecs(std::string fname, size_t* dim, size_t* vecnum);
int* read_ivecs(std::string fname, size_t* dim, size_t* vecnum);
#endif //DATABASE_H
