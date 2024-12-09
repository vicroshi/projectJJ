#include <iostream>
#include <fstream>
#include <filesystem>


int main() {
    // Get the current path of the executable
    std::filesystem::path current_path = std::filesystem::current_path();
    // std::cout<<current_path<<std::endl;

    std::filesystem::path temp=current_path;

    while (!temp.empty() && temp.filename() != "projectJJ") {
        temp = temp.parent_path();
    }

    // Find the project root directory
    std::filesystem::path project_root = temp;

    if (project_root.empty()) {
        std::cerr << "Project root 'projectJJ' not found." << std::endl;
        return 1;
    }

    // Construct the desired file path
    std::filesystem::path desired_directory = project_root / "graphs" ;
    int variable1 = 42;
    double variable2 = 1.2;


    // Convert variables to strings
    std::string variable1_str = std::to_string(variable1);
    std::string variable2_str = std::to_string(variable2);
    if (variable2_str.length() > 3) {
        variable2_str = variable2_str.substr(0, 3);
    }

    std::ostringstream file_name_stream;
    file_name_stream << "file_" << variable1_str << "_" << variable2_str << ".txt";
    std::string file_name = file_name_stream.str();

    // Construct the desired file path
    std::filesystem::path desired_file = desired_directory / file_name;
    // Check if the directory exists, if not create it
    if (!std::filesystem::exists(desired_directory)) {
        std::filesystem::create_directories(desired_directory);
        std::cout << "Directory created: " << desired_directory << std::endl;
    }
    return 0;
}