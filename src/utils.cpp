#include <iostream>
#include <string>
#include "utils.h"
#include "graph.h"
#include <iterator> // for std::back_inserter
#include <fstream>
#include <filesystem>

std::string getFileExtension(const std::string& filePath){
    // Find the last dot in the file path
    size_t dotPosition = filePath.find_last_of(".");
    // If there's no dot or the dot is the first character (eg: .git),return an empty string
    if (dotPosition == std::string::npos || dotPosition == 0 || dotPosition == filePath.length() - 1) {
        return "";
    }
    // Extract the substring after the last dot
    return filePath.substr(dotPosition + 1);
}

void ann(const std::string& base_file_path, const std::string& query_file_path,const std::string& ground_file_path,const float& a, const size_t& k,const size_t& R,const size_t& List_size,int thread_num){
    //makes sense to first check if the ground_truth is the right type of file
    omp_set_num_threads(thread_num);

    if(getFileExtension(ground_file_path)=="ivecs"){
        //check for file type to call the execute with proper <type>
        //if user gives a .fvecs base file and a .ivecs as query, should exit
        std::string type=getFileExtension(base_file_path);
        if(type==getFileExtension(query_file_path) && type=="fvecs"){
            execute<float>(base_file_path,query_file_path,ground_file_path,a,k,R,List_size,thread_num);
        }
        else if(type==getFileExtension(query_file_path) && type=="ivecs"){
            execute<int>(base_file_path,query_file_path,ground_file_path,a,k,R,List_size,thread_num);
        }
        //any other case: they're incompatible
        else {
            std::cout<<"Incompatible input files!"<<std::endl;
        } 
    }else{
        std::cout<<"Unsupported ground_truth file type!"<<std::endl;
    } 
    return;
}

double recall_k(const int& k, std::vector<int>& X, std::vector<int>& G,int prnt=0){
    if (!k) {
        return !X.size()? 1.0f : 0.0f;
    }
    std::vector<int>intersection;
    std::sort(X.begin(), X.end());
    std::sort(G.begin(), G.end());
    std::set_intersection(X.begin(), X.end(), G.begin(), G.end(),std::back_inserter(intersection));

    if(prnt){
        std::cout<<"\nG:[";
        for(auto i:G){
            std::cout<<i<<" ";
        }
        std::cout<<"]\n\n";

        std::cout<<"\nX:[";
        for(auto i:X){
            std::cout<<i<<" ";
        }
        std::cout<<"]\n\n";
    }
    
    
    // if(prnt) std::cout<<"recall: "<< (double) intersection.size()/(double) k <<std::endl;
    return (double) intersection.size()/(double) k;
}

std::filesystem::path get_file_path(const size_t& k, const size_t& L, const size_t& R, const float& a,std::string type, const size_t & file_size){
    std::filesystem::path current_path = std::filesystem::current_path();

        std::filesystem::path temp=current_path; //find common parent folder
        while (!temp.empty() && temp.filename() != "projectJJ") {
            temp = temp.parent_path();
        }

        // Find the project root directory
        std::filesystem::path project_root = temp;

        // Construct the desired dir path,create dir if not already existent
        std::filesystem::path desired_directory = project_root / "graphs" ;
        
        if (!std::filesystem::exists(desired_directory)) {
            std::filesystem::create_directories(desired_directory);
            std::cout << "Directory created: " << desired_directory << std::endl;
        }

        //create file path.
        std::string a_str = std::to_string(a);
        std::string k_str = std::to_string(k);
        std::string L_str = std::to_string(L);
        std::string R_str = std::to_string(R);
        std::replace(a_str.begin(), a_str.end(), '.', '_');
        while (a_str.back() == '0') {
            a_str.pop_back();
        }
        if (a_str.length() > 4){
                a_str = a_str.substr(0, 4);
        }
        if(a_str.find('_') == a_str.length() -1 ){
            a_str.pop_back();    
        }
        // std::cout << a_str << std::endl;
        //construct file name
        std::ostringstream file_name_stream;
        file_name_stream << type << file_size<<"_k_" << k_str << "_" << (type=="stitched_graph_" ? "L_small_" : "L_") <<L_str << "_" <<(type=="stitched_graph_" ? "R_small_" : "R_") <<R_str<<"_a_"<<a_str<<"_" << "variation.bin";
        std::string file_name = file_name_stream.str();

        // Construct the desired file path
        std::filesystem::path desired_file = desired_directory / file_name;
        return desired_file;
}