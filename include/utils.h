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
#include "graph.h"


void ann();
std::string getFileExtension( const std::string& );


template <typename T>
void execute(std::string base_file_path,std::string query_file_path,int k,int a,int L,int R){
    size_t base_dim,base_vecs_num;
    T* base=read_from_file<T>(&base_file_path,&base_dim,&base_vecs_num);
    //std::span<T> db(base, base_dim*base_vecs_num);
    Matrix<T> base_m(base_dim,base_vecs_num,base);
    VamanaIndex<T> v_m(R,&base_m);
    // std::cout<<"[";
    // for (size_t i=0;i<100;i++){
    //     std::cout<<db[i]<<" ";
    // }
    // std::cout<<"]"<<std::endl;
    size_t query_dim,query_vecs_num;
    T* query=read_from_file<T>(&query_file_path,&query_dim,&query_vecs_num);
    //std::span<T> qr(query, query_dim*query_vecs_num);
    Matrix<T> query_m(query_dim,query_vecs_num,query);
    // std::cout<<"[";
    // for (size_t i=0;i<100;i++){
    //     std::cout<<qr[i]<<" ";
    // }
    // std::cout<<"]"<<std::endl;
    
    // v_m.print_graph();

    std::cout<<"about to exit execute..."<<std::endl;
}

template <typename T>
T* read_from_file(std::string* name, size_t* dim, size_t* vecnum) {
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
    T* p = (T*)mmap(nullptr, sb.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
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