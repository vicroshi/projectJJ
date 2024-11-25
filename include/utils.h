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
#include <iterator> // for std::back_inserter


void ann();
std::string getFileExtension(const std::string&);

template <typename T>
T* read_from_file(const std::string&,size_t*,size_t*);

double recall_k(const int&,std::vector<int>&,std::vector<int>&);

template <typename T>
void execute(const std::string& base_file_path,const std::string& query_file_path,const std::string& ground_file_path){

    //file reading and storing in matrixes
    size_t base_dim,base_vecs_num;
    T* base=read_from_file<T>(base_file_path,&base_dim,&base_vecs_num);
    Matrix<T> base_m(base_dim,base_vecs_num,base);

    size_t query_dim,query_vecs_num;
    T* query=read_from_file<T>(query_file_path,&query_dim,&query_vecs_num);
    Matrix<T> query_m(query_dim,query_vecs_num,query);

    size_t ground_dim,ground_vecs_num;
    int* ground=read_from_file<int>(ground_file_path,&ground_dim,&ground_vecs_num);
    Matrix<int> ground_m(ground_dim,ground_vecs_num,ground);

    std::cout<<"base vec num:"<<base_vecs_num<<std::endl;
    std::cout<<"query vec num:"<<query_vecs_num<<std::endl;
    std::cout<<"ground vec num:" << ground_vecs_num<<std::endl;

    //parameters input
    size_t k, List_size, R;
    float a;
    // Get valid integer input for `k`
    while(true){
        std::cout<<"k: ";
        std::cin>>k;
        if(std::cin.fail() || k<=0 || k>base_vecs_num){
            std::cin.clear();  // clear error state
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');  // discard invalid input
            std::cout<<"Invalid input. Please submit a positive integer in range [1,"<<base_vecs_num<<"]\n";
        }else
            break;  //valid input received
        
    }
    // Get valid integer input for `alpha`
    while(true){
        std::cout<<"alpha: ";
        std::cin>>a;
        if(std::cin.fail() || a<1.0f){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Invalid input. Please submit a float greater or equal to 1.\n";
        }else
            break;
        
    }
    // Get valid integer input for Lise_size
    while(true){
        std::cout<<"List size (L): ";
        std::cin>>List_size;
        if(std::cin.fail() || List_size<=0){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Invalid input. Please submit a positive integer.\n";
        }else
            break;
        
    }
    // Get valid integer input for `R`
    while(true){
        std::cout<<"Out-degree R: ";
        std::cin>>R;
        if(std::cin.fail() || R<=0 || R>=base_vecs_num){
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');
            std::cout << "Invalid input. Please submit a positive integer in range [1,"<<base_vecs_num-1<<"]\n";
        }else
            break;
        
    }

    //times for each action

    //R-regular graph initialization
    auto init_start = std::chrono::high_resolution_clock::now();
    VamanaIndex<T> v_m(R,&base_m);
    auto init_end = std::chrono::high_resolution_clock::now();
    auto init_duration = std::chrono::duration_cast<std::chrono::microseconds>(init_end - init_start).count();
    std::cout << ">Time taken to initialize "<<R<<"-regular Graph: " << init_duration / 1e6 << " sec(s)." << std::endl;
    
    //calculate medoid once and pass it to functions later
    auto medoid_start = std::chrono::high_resolution_clock::now();
    int medoid=v_m.db->medoid_naive();
    auto medoid_end = std::chrono::high_resolution_clock::now();
    auto medoid_duration = std::chrono::duration_cast<std::chrono::microseconds>(medoid_end - medoid_start).count();
    std::cout << ">Time taken to find medoid: " << medoid_duration / 1e6 << " sec(s)." << std::endl;

    //actual indexing
    auto indexing_start = std::chrono::high_resolution_clock::now();
    v_m.vamana_indexing(medoid,a,List_size,R);
    auto indexing_end = std::chrono::high_resolution_clock::now();
    auto indexing_duration = std::chrono::duration_cast<std::chrono::microseconds>(indexing_end - indexing_start).count();
    std::cout << ">Time taken for Indexing: " << indexing_duration / 1e6 << " sec(s)." << std::endl;

    //print overall recall first
    double sum=0.0;
    for(size_t i=0;i<query_vecs_num;i++){
        std::span<T> query_span(query_m.row(i));
        std::unordered_set<int>L,V;
        v_m.greedy_search(medoid,query_span,k,List_size,L,V);
        auto row=ground_m.row(i);
        std::vector<int> G(row.begin(),row.begin()+k);
        std::vector<int>vecL(L.begin(),L.end());
        sum+=recall_k(k,vecL,G);
    }
    std::cout<<"Averall recall: "<<sum/(double) query_vecs_num<<"\n";


    //get input in a loop to show k-nearest neighbors for any query in file
    int query_point_index;
    //get valid integer input for query input, indexing starts at 0
    while(true){
    std::cout<<"Enter query index (-1 to exit): ";
    std::cin>>query_point_index;
        //check if -1 was given at start
        if(query_point_index==-1)
            break;
        
        //inner loop to validate input
        while(std::cin.fail() || query_point_index<0 || (size_t) query_point_index>=query_vecs_num){
            std::cin.clear();  // clear error flag
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(),'\n');  // discard invalid input
            std::cout<<"Please enter a positive integer in range [0,"<<query_vecs_num-1<<"] or -1 to exit: ";
            std::cin>>query_point_index;
            //check again if the user entered -1 to exit
            if(query_point_index ==-1)
                break;
            
        }
        //if -1 was entered during inner validation loop,break out of the outer loop as well
        if(query_point_index==-1)
            break;
        

        //run greedy search to return the k-closest of requested query node
        std::span<T> query_span(query_m.row(query_point_index));
        std::unordered_set<int>L,V;
        v_m.greedy_search(medoid,query_span,k,List_size,L,V);
        std::cout<<"[ ";
        for(auto item:L)
            std::cout<<item<<" ";
        std::cout<<" ]\n";
        auto row=ground_m.row(query_point_index);
        std::vector<int> G(row.begin(),row.begin()+k);
        std::vector<int>vecL(L.begin(),L.end());
        std::cout<<"recall:"<<recall_k(k,vecL,G)<<std::endl;
    }

    //free mmaps, no leaks whatsoever?? 
    size_t mapped_size = (base_dim + 1) * base_vecs_num * 4;
    if (munmap(base, mapped_size) == -1) {
        std::cerr << "Error unmapping the memory" << std::endl;
    }
    mapped_size= (query_dim + 1) * query_vecs_num * 4;
    if (munmap(query, mapped_size) == -1) {
        std::cerr << "Error unmapping the memory" << std::endl;
    }
    mapped_size= (ground_dim + 1) * ground_vecs_num * 4;
    if (munmap(ground, mapped_size) == -1) {
        std::cerr << "Error unmapping the memory" << std::endl;
    }

}

//function to read a file and store it in a flat array for efficiency
template <typename T>
T* read_from_file(const std::string& name, size_t* dim, size_t* vecnum) {
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
    *vecnum = sb.st_size / ((d + 1) * sizeof(T));
    for (size_t i = 0; i < *vecnum; i++) {
        std::memmove(p + i * d, p + 1 + i * (d + 1), d * sizeof(*p));
    }
    return p;
}