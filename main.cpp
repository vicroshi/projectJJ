#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <random>
#include <sys/stat.h>
#include <iomanip> // Include iomanip for setprecision
#include "graph.h"
#include "database.h"
#include "utils.h"

int main(int argc,char **argv){
    int opt;
    float a=0.0;
    size_t R=0,L=0,k=0;
    std::string base_file_path,query_file_path,ground_file_path;
    while((opt = getopt(argc, argv, "a:R:L:b:q:g:k:"))!=-1){
        try{
            switch(opt){
                case 'a':
                    a=std::stof(optarg);
                    break;
                case 'R':
                    R=std::stoul(optarg);
                    break;
                case 'L':
                    L=std::stoul(optarg);
                    break;
                case 'b':
                    base_file_path=optarg;
                    break;
                case 'q':
                    query_file_path=optarg;
                    break;
                case 'g':
                    ground_file_path=optarg;
                    break;
                case 'k':
                    k=std::stoul(optarg);
                    break;
                default:
                    throw std::invalid_argument("Invalid option");
            }
        //something was given incorrectly
        }catch(const std::invalid_argument& e){
            
            std::cerr << "Error: Invalid argument for option -" << static_cast<char>(optopt) << ": " << optarg << std::endl;
            std::cerr << "Usage: ./projectjj -a <value> -R <value> -L <value> -b <value> -q <value> -g <value>" << std::endl;
            return 1;
        }
    }
    // Check if file paths are not empty
    if(base_file_path.empty() || query_file_path.empty() || ground_file_path.empty()){
        std::cerr << "Usage: ./projectjj -a <value> -R <value> -k <value> -L <value> -base <value> -query <value> -ground <value>" << std::endl;
        return 1;
    }

    //call ann() here to check file type for first project and then call execute
    ann(base_file_path,query_file_path,ground_file_path,a,k,R,L);


    return 0;
}

