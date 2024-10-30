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
T* read_from_file(const std::string&, size_t*, size_t*);

inline float recall(int k, std::set<int>& X, std::set<int>& T) {
    std::set<int> intrsct;
    for (auto &i : X) {
        std::cout << i << " ";
    }
    std::cout << std::endl;
    for (auto &i : T) {
        std::cout << i << " ";
    }
//    std::ranges::set_intersection(X, T, std::inserter(intrsct, intrsct.begin()));
    std::set_intersection(X.begin(), X.end(), T.begin(), T.end(), std::inserter(intrsct, intrsct.begin()));
    return static_cast<float>(intrsct.size()) / static_cast<float>(k);
}
// 81 99 6110 6159 6183 6645 6719 6819 6997 7633
// 11 69 235 279 384 414 528 647 711 763 855 965 1076 1088 1118 1252 1262 1269 1274 1365 1398 1477 1661 1746 1889 1891 1909 1912 1914 1950 2016 2142 2247 2377 2428 2473 2482 2692 2891 2894 2919 3254 3270 3434 3631 3689 3730 3748 3899 4086 4151 4334 4344 4402 4440 4677 4692 4734 4761 4767 4910 5072 5096 5134 5167 5252 5310 5327 5421 5776 6142 6234 6661 7344 7428 7857 8028 8082 8102 8414 8475 8504 8575 8782 8852 8896 8976 9045 9108 9163 9240 9308 9338 9494 9846 9892 9942 9951 9987 9993

//../datasets/siftsmall/siftsmall_base.fvecs
//../datasets/siftsmall/siftsmall_query.fvecs
//../datasets/siftsmall/siftsmall_groundtruth.ivecs
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
    
    // v_m.print_graph();

    std::cout<<"about to exit execute..."<<std::endl;
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