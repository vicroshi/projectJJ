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
#include <set>

void ann();
std::string getFileExtension( const std::string& );

template <typename T>
T* read_from_file(std::string*, size_t*, size_t*);

template <typename T>
void execute(std::string base_file_path,std::string query_file_path,int k,float a,int L,int R){
    size_t base_dim,base_vecs_num;
    T* base=read_from_file<T>(&base_file_path,&base_dim,&base_vecs_num);
    //std::span<T> db(base, base_dim*base_vecs_num);
    Matrix<T> base_m(base_dim,base_vecs_num,base);
    VamanaIndex<T> v_m(R,&base_m);
    std::vector<T> eg={0,16,35,5,32,31,14,10,11,78,55,10,45,83,11,6,14,57,102,75,20,8,3,5,67,17,19,26,5,0,1,22,60,26,7,1,18,22,84,53,85,119,119,4,24,
        18,7,7,1,81,106,102,72,30,6,0,9,1,9,119,72,1,4,33,119,29,6,1,0,1,14,52,119,30,3,0,0,55,92,111,2,5,4,9,22,89,96,14,1,0,1,82,59,16,20,5,25,14,11,4,0,0,1,26,47,
        23,4,0,0,4,38,83,30,14,9,4,9,17,23,41,0,0,2,8,19,25,23,1};
    std::span<T> spanEg(eg.data(), eg.size());
    std::set<int> l,V;
    // v_m.greedy_search(4,spanEg,k,(size_t) L,l,V);
    v_m.robust_prune(2,V,a,(size_t) R);


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
        std::cerr<<"Error opening file for reading"<<std::endl;
        return nullptr;
    }
    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        std::cerr<<"Error getting the file size"<<std::endl;
        return nullptr;
    }
    T* p = (T*)mmap(nullptr, sb.st_size, PROT_READ|PROT_WRITE, MAP_PRIVATE, fd, 0);
    if (p == MAP_FAILED) {
        std::cerr<<"Error mapping the file"<<std::endl;
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