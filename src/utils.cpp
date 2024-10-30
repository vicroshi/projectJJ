#include "database.h"
#include <iostream>
#include <string>
#include <limits>
#include "utils.h"
#include "database.h"
#include "graph.h"

std::string getFileExtension(const std::string& filePath) {
    // Find the last dot in the file path
    size_t dotPosition = filePath.find_last_of(".");
    
    // If there's no dot or the dot is the first character (e.g., ".hiddenfile"), return an empty string
    if (dotPosition == std::string::npos || dotPosition == 0 || dotPosition == filePath.length() - 1) {
        return "";
    }
    // Extract the substring after the last dot
    return filePath.substr(dotPosition + 1);
}


void ann(){
    std::string base_file_path, query_file_path;
    int k, L, R;
    float a;
    while (true) {
        std::cout << "base file path: ";
        std::cin >> base_file_path;
        std::cout << "query file path: ";
        std::cin >> query_file_path;
        // Get valid integer input for `k`
        while (true) {
            std::cout << "k: ";
            std::cin >> k;
            if (std::cin.fail() || k <= 0) {
                std::cin.clear();  // clear error state
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // discard invalid input
                std::cout << "Invalid input. Please submit a positive integer.\n";
            }else{
                break;  // valid input received
            }
        }
        // Get valid integer input for `alpha`
        while (true) {
            std::cout << "alpha: ";
            std::cin >> a;
            if (std::cin.fail() || a < 1.0f) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please submit a float greater or equal to 1.\n";
            } else {
                break;
            }
        }
        // Get valid integer input for `L`
        while (true) {
            std::cout << "List size (L): ";
            std::cin >> L;
            if (std::cin.fail() || L <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please submit a positive integer.\n";
            } else {
                break;
            }
        }
        // Get valid integer input for `R`
        while (true) {
            std::cout << "Out-degree R: ";
            std::cin >> R;
            if (std::cin.fail() || R <= 0) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Invalid input. Please submit a positive integer.\n";
            } else {
                break;
            }
        }
        //check for file type to call the execute with proper <type>
        //if user gives a .fvecs base file and a .ivecs as query, should exit
        std::string type=getFileExtension(base_file_path);
        if(type==getFileExtension(query_file_path) && type=="fvecs"){
            execute<float>(base_file_path,query_file_path,k, a, L, R);
        }
        else if(type==getFileExtension(query_file_path) && type=="ivecs"){
            execute<int>(base_file_path,query_file_path,k, a, L, R);
        }
        //any other case: they're incompatible
        else {
            std::cout<<"Incompatible input files!\nExiting..."<<std::endl;
            break;
        }
        break;
//        std::string exit;
//        std::cout << "Stop execution? (yes/no): ";
//        std::cin >> exit;
//        if (exit.starts_with("y") || exit.starts_with("Y"))

//            break;
//        }
    }
}
