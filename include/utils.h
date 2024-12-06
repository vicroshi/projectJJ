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


void ann(const std::string& , const std::string& ,const std::string& ,const float& , const size_t& ,const size_t& ,const size_t& );
std::string getFileExtension(const std::string&);

template <typename T>
T* read_from_file(const std::string&,size_t*,size_t*);

double recall_k(const int&,std::vector<int>&,std::vector<int>&);

template <typename T>
void execute(const std::string& base_file_path,const std::string& query_file_path,const std::string& ground_file_path,const float& a, const size_t& k,const size_t& R,const size_t& List_size){
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

    if(k<=0 || k>base_vecs_num){
        std::cout<<"Invalid input for k.Please submit a positive integer in range [1,"<<base_vecs_num<<"]\n";
        exit(1);
    }

    if(a<1.0f){
        std::cout<<"Invalid input for alpha. Please submit a float greater or equal to 1.\n";
        exit(1);
    }

    if(List_size<=0){
        std::cout<<"Invalid input for List size. Please submit a positive integer.\n";
        exit(1);
    }

    if(R<=0 || R>=base_vecs_num){
        std::cout<<"Invalid input for R. Please submit a positive integer in range [1,"<<base_vecs_num-1<<"]\n";
        exit(1);
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

//overloading execute for second project
template <typename T>
void execute(const std::string& base_file_path,const std::string& query_file_path,const std::string& ground_file_path,const float& a, const size_t& k,const size_t& R,const size_t& List_size,const size_t& t){

    // database read
    std::vector<std::vector<T>> base_data; // only used for reading, will be dropped after data extraction
    std::vector<T> flat_base;
    uint32_t base_no_of_points;
    ReadBin(base_file_path, 102, base_data, base_no_of_points);

    // data extraction
    std::vector<T> base_filter;        // to filter kathe point
    std::unordered_set<T> filters_set; // ola ta filters pou uparxoun
    base_filter.reserve(base_no_of_points);
    filters_set.reserve(base_no_of_points / 5); // rough estimate
    extract_base_vector_info(base_data, base_filter, filters_set, flat_base);

    // database init
    Matrix<T> base_m(static_cast<size_t>(100), base_no_of_points, &flat_base, &base_filter, &filters_set);
    // base_m.print_check();

    // query read
    std::vector<std::vector<T>> query_data; // only used for reading, will be dropped after data extraction
    std::vector<T> flat_query;
    uint32_t query_no_of_points;
    ReadBin(query_file_path, 104, query_data, query_no_of_points);

    // data extraction
    std::vector<T> query_filter;
    query_filter.reserve(query_no_of_points);
    std::vector<int> query_type;
    query_type.reserve(query_no_of_points);
    size_t num_filtered_points=0,num_unfiltered_points=0;
    extract_query_vector_info(query_data, query_filter, query_type, flat_query,num_filtered_points,num_unfiltered_points);

    // database init                                                        //no need to keep all filters used in queries, we have them from DB.
    Matrix<T> query_m(static_cast<size_t>(100), query_no_of_points, &flat_query, &query_filter, NULL);

    
    // groundtruth read
    uint32_t ground_no_of_points;
    std::vector<std::vector<float>> ground_data_float;
    std::vector<std::vector<int>> ground_data;
    ReadBin(ground_file_path, 100, ground_data_float, ground_no_of_points);
    RemoveNegativeElements(ground_data_float,ground_data); // format it as needed because binary is read with readBin and dimensions must be constant for each vector
    

    // parameters input
    // size_t k, List_size, R, t;
    size_t L_small, R_small, R_stitched;
    // float a;
    

    // times for each action

    // R-regular graph initialization
    auto init_start = std::chrono::high_resolution_clock::now();
    VamanaIndex<T> v_m(&base_m);
    auto init_end = std::chrono::high_resolution_clock::now();
    auto init_duration = std::chrono::duration_cast<std::chrono::microseconds>(init_end - init_start).count();
    std::cout << ">Time taken to initialize Graph: " << init_duration / 1e6 << " sec(s)." << std::endl;
    

    // calculate medoid once and pass it to functions later
    auto medoid_start = std::chrono::high_resolution_clock::now();
    std::unordered_map<float, int> Medoid;          // st(f)
    std::unordered_map<float, std::vector<int>> Pf; // points for each filter
    v_m.db->FindMedoid(t, Medoid, Pf);

    auto medoid_end = std::chrono::high_resolution_clock::now();
    auto medoid_duration = std::chrono::duration_cast<std::chrono::microseconds>(medoid_end - medoid_start).count();
    std::cout << ">Time taken to find medoid: " << medoid_duration / 1e6 << " sec(s)." << std::endl;

    // actual indexing
    auto indexing_start = std::chrono::high_resolution_clock::now();
    v_m.FilteredVamanaIndexing(Medoid, a, List_size, R);
    auto indexing_end = std::chrono::high_resolution_clock::now();
    auto indexing_duration = std::chrono::duration_cast<std::chrono::microseconds>(indexing_end - indexing_start).count();
    std::cout << ">Time taken for Indexing: " << indexing_duration / 1e6 << " sec(s)." << std::endl;

    double sum_filtered=0.0f,sum_unfiltered=0.0f;

    for (uint32_t i = 0; i < query_no_of_points; i++){
        if(query_type[i]<2.0f){
            // std::cout<<"for i:"<<i<<",with filter:"<<(*query_m.vec_filter)[i] <<std::endl;
            std::unordered_set<int> L, V;
            std::span<T> query_span(query_m.row(i));
            v_m.FilteredGreedySearch(Medoid, query_span, k, List_size, (*query_m.vec_filter)[i], L, V);
            std::vector<int> L_vec(L.begin(), L.end());
                std::cout<<"==============================\n"<<"for i:"<<i<<" with filter:"<<(*query_m.vec_filter)[i]<< ", L.size():"<<L.size()<<", ground.size():"<<ground_data[i].size()<<"\n";
                // std::cout<<"L:";
                // for(auto j:L_vec){
                //     std::cout<<j<<" ";
                // }
                // std::cout<<"\n";
                // std::cout<<"G:";
                // for(auto j:ground_data[i]){
                //     std::cout<<j<<" ";
                // }
                std::cout<<"\n";
                // size_t common_elements = 0;
                // for (const auto &elem : ground_data[i]) {
                //     if (std::find(L_vec.begin(), L_vec.end(), elem) != L_vec.end()) {
                //         common_elements++;
                //     }
                
                //     if(common_elements != ground_data[i].size()) std::cout<<"!NOT identical\n";
                //     std::cout << "Number of common elements: " << common_elements <<"\n";
                // }
                size_t n = std::min(k, ground_data[i].size());
                std::vector<int> G_vec(ground_data[i].begin(),ground_data[i].begin()+n);
                if(query_type[i]==1.0f){
                    auto recall=recall_k(n,L_vec,G_vec);
                    std::cout << std::fixed << std::setprecision(2);
                    std::cout<<"--recall:"<<recall<<std::endl;
                    sum_filtered+=recall;
                }
                else if(query_type[i]==0.0f){
                    auto recall=recall_k(n,L_vec,G_vec);
                    std::cout << std::fixed << std::setprecision(2);
                    std::cout<<"--recall:"<<recall<<std::endl;
                    sum_unfiltered+=recall;
                }

                std::cout<<"=============================="<< std::endl<<std::endl;
        }
    }
    std::cout << "sum_filtered: " << sum_filtered << ", num_filtered_points: " << num_filtered_points << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "TOTAL recall for filtered: " << sum_filtered / static_cast<double>(num_filtered_points) << std::endl;
    std::cout << "sum_unfiltered: " << sum_unfiltered << ", num_unfiltered_points: " << num_unfiltered_points << std::endl;
    std::cout << "TOTAL recall for unfiltered: " << sum_unfiltered / static_cast<double>(num_unfiltered_points) << std::endl;
    std::cout<<"\n\n--END--";

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