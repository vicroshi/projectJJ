#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include <random>
#include "utils.h"

int main(int argc,char **argv){
    int opt;
    float a=0.0;
    size_t R=0,L=0,k=0,t=0,R_small=0,L_small=0;
    int load=0,save=0;
    std::string base_file_path,query_file_path,ground_file_path;
    while((opt = getopt(argc, argv, "a:R:L:k:b:q:g:t:r:l:os"))!=-1){
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
                case 't':
                    t=std::stoul(optarg);
                    break;
                case 'o': //to load
                    load = 1;
                    break;
                case 's':
                    save = 1;
                    break;
                case 'r':
                    R_small = std::stoul(optarg);
                    break;
                case 'l':
                    L_small = std::stoul(optarg);
                    break;
                default:
                    throw std::invalid_argument("Invalid option");
            }
        //something was given incorrectly
        }catch(const std::invalid_argument& e){
            std::cerr << "Error: Invalid argument for option -" << static_cast<char>(optopt) << ": " << optarg << std::endl;
            std::cerr << "Usage: ./projectJJ_filtered -a <value> -R <value> -k <value> -L <value> -t <value> -b (base file) <value> -q (query file) <value> -g (ground file) <value> -r (R_small) <value> -l (L_small) <value> -o (load) -s (save)" << std::endl;
            return 1;
        }
    }
    //cannot load and save in the same execution
    if(save && load){
        std::cerr << "Cannot load and save in the same execution,aborting."<< std::endl;
        return 1;
    }


    // Check if file paths are not empty
    if(base_file_path.empty() || query_file_path.empty() || ground_file_path.empty()){
        std::cerr << "Usage: ./projectJJ_filtered -a <value> -R <value> -k <value> -L <value> -t <value> -b (base file) <value> -q (query file) <value> -g (ground file) <value> -r (R_small) <value> -l (L_small) <value> -o (load) -s (save)" << std::endl;
        return 1;
    }

    //call immediately execute for filtered vamana
    execute<float>(base_file_path,query_file_path,ground_file_path,a,k,R,L,t,R_small,L_small,load,save);


    return 0;
}

