#include "database.h"
#include <iostream>
#include <string>
#include <limits>
#include "utils.h"
#include "database.h"
#include "graph.h"
#include <iterator> // for std::back_inserter

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

void ann(){
    std::string base_file_path, query_file_path,ground_file_path;

    //used for testing to not have to insert it every time
    // base_file_path="/home/zenki/Documents/DIT/jj/projectJJ/datasets/siftsmall/siftsmall_base.fvecs";
    // query_file_path="/home/zenki/Documents/DIT/jj/projectJJ/datasets/siftsmall/siftsmall_query.fvecs";
    // ground_file_path="/home/zenki/Documents/DIT/jj/projectJJ/datasets/siftsmall/siftsmall_groundtruth.ivecs";

    std::cout << "base file path: ";
    std::cin >> base_file_path;
    std::cout << "query file path: ";
    std::cin >> query_file_path;
    std::cout<< "ground file path: ";
    std::cin >> ground_file_path;
    //makes sense to first check if the ground_truth is the right type of file
    if(getFileExtension(ground_file_path)=="ivecs"){
        //check for file type to call the execute with proper <type>
        //if user gives a .fvecs base file and a .ivecs as query, should exit
        std::string type=getFileExtension(base_file_path);
        if(type==getFileExtension(query_file_path) && type=="fvecs"){
            execute<float>(base_file_path,query_file_path,ground_file_path);
        }
        else if(type==getFileExtension(query_file_path) && type=="ivecs"){
            execute<int>(base_file_path,query_file_path,ground_file_path);
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

double recall_k(const int& k, std::vector<int>& X, std::vector<int>& G){
    std::vector<int>intersection;
    std::sort(X.begin(), X.end());
    std::sort(G.begin(), G.end());
    std::set_intersection(X.begin(), X.end(), G.begin(), G.end(),std::back_inserter(intersection));
    
    // std::cout<<"recall: "<< (double) intersection.size()/(double) k <<std::endl;
    return (double) intersection.size()/(double) k;
}


