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
#include <chrono> //for timing

void ann();
std::string getFileExtension( const std::string& );

template <typename T>
T* read_from_file(const std::string&, size_t*, size_t*);

void recall_k(int ,std::set<int> , std::set<int> );



template <typename T>
void execute(std::string base_file_path,std::string query_file_path,int k,float a,int L,int R){
    size_t base_dim,base_vecnum = 0;
    T* base=read_from_file<T>(base_file_path,&base_dim,&base_vecnum);
    //std::span<T> db(base, base_dim*base_vecs_num);
    Matrix<T> base_m(base_dim,base_vecnum,base);
    size_t gt_dim, gt_vecnum = 0;
    std::string gt_file_path = "../datasets/siftsmall/siftsmall_groundtruth.ivecs";
    int* ground_truth = read_from_file<int>(gt_file_path, &gt_dim, &gt_vecnum);
    Matrix<int> gt_m(gt_dim, gt_vecnum, ground_truth);
    VamanaIndex<T> v_m(R,L,k,a,&base_m);
    std::set<int> l,v={};
    size_t query_dim,query_vecnum = 0;
    T* query=read_from_file<T>(query_file_path,&query_dim,&query_vecnum);
    Matrix<T> query_m(base_dim,query_vecnum,query);
//    std::cout << "Choose a query (type a number from 1 to " << query_vecnum << ")\n";
//    int query_num;
//    std::cin >> query_num ;
//    int query_idx = query_num - 1;
    for (int query_idx = 0; query_idx < query_vecnum; query_idx++) {
        v_m.greedy_search(MEDOID,base_m.get_row(query_idx),k,L, l ,v);
        auto gt = gt_m.get_row(query_idx);
        std::set<int> t(gt.begin(), gt.end());
        printf("query #%d %d-recall@%d: %f\n",query_idx,k,L,recall(k,l,t));
    }
    munmap(base, base_dim * base_vecnum * sizeof(T));
    munmap(ground_truth, gt_dim * gt_vecnum * sizeof(int));
    munmap(query, query_dim * query_vecnum * sizeof(T));
//    v_m.greedy_search(0,base_m.get_row(query_idx),k,L, l ,v);
//    auto gt = gt_m.get_row(query_idx);
//    std::set<int> t(gt.begin(), gt.end());
//    printf("query #%d %d-recall@%d: %f\n",query_num,k,L,recall(k,l,t));
    //    v_m.greedy_search(0,base_m.get_row(0),k,L, l ,v);
    // std::cout<<"[";
    // for (size_t i=0;i<100;i++){
    //     std::cout<<db[i]<<" ";
    // }
    // std::cout<<"]"<<std::endl;
//    size_t query_dim,query_vecs_num;
//    T* query=read_from_file<T>(&query_file_path,&query_dim,&query_vecs_num);
    //std::span<T> qr(query, query_dim*query_vecs_num);
//    Matrix<T> query_m(query_dim,query_vecs_num,query);
    // std::cout<<"[";
    // for (size_t i=0;i<100;i++){
    //     std::cout<<qr[i]<<" ";
    // }
    // std::cout<<"]"<<std::endl;
    Matrix<T> base_m(base_dim,base_vecs_num,base);
    VamanaIndex<T> v_m(R,&base_m);

    size_t query_dim,query_vecs_num;
    T* query=read_from_file<T>(&query_file_path,&query_dim,&query_vecs_num);
    Matrix<T> query_m(query_dim,query_vecs_num,query);

    std::span<T> q_span(query_m.get_row(1));
    // for(auto item:q_span) std::cout<<item<<" ";

    auto clock_start = std::chrono::high_resolution_clock::now();
    v_m.vamana_indexing(a,list_size,R);
    std::set<int>L,V;
    v_m.greedy_search(MEDOID,q_span,k,list_size,L,V);
    auto clock_end = std::chrono::high_resolution_clock::now();

    std::cout<<"[ ";
    for(auto item:L){
        std::cout<<item<<" ";
    }
    std::cout<<" ]\n";

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(clock_end - clock_start).count();
    std::cout << "Time taken: " << duration / 1e6 << " sec(s)." << std::endl;
    size_t ground_dim,ground_vecs_num;
    std::string ground_path("../datasets/siftsmall/siftsmall_groundtruth.ivecs");
    int* ground=read_from_file<int>(&ground_path,&ground_dim,&ground_vecs_num);
    Matrix<int> ground_m(ground_dim,ground_vecs_num,ground);
    auto row=ground_m.get_row(1);
    std::set<int> G(row.begin(),row.begin()+k);

    recall_k(k,L,G);


}

template <typename T>
T* read_from_file(const std::string &name, size_t* dim, size_t* vecnum) {
    int fd = open(name.c_str(), O_RDONLY);
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
    for (size_t i = 0; i < *vecnum; i++) {
        std::memmove(p + i * d, p + 1 + i * (d + 1), d * sizeof(*p));
    }
    return p;
}