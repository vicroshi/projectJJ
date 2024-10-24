#include <fcntl.h>    // For open() and file flags
#include <unistd.h>   // For read(), write(), close()
#include <iostream>
#include <cstring>
#include <string>
#include <vector>
#include <variant>   // For std::variant
#include <iomanip> 

using namespace std;
using TypeVariant = std::variant<int, float>;

// Function to print the contents of a 2D vector of variants
void print_vector(const vector<vector<TypeVariant>>& vec) {
    for (size_t i = 0; i < vec.size(); ++i) {
        // cout << "Vector " << i + 1 << ": ";
        for (int j=0;j<vec[i].size();j++) {
            // Check the type of each value and print accordingly
            if (holds_alternative<int>(vec[i][j])) {
                cout << get<int>(vec[i][j]) << " "; // Print integer
            } else if (holds_alternative<float>(vec[i][j])) {
                cout << get<float>(vec[i][j]) << " "; // Print float
            }
        }
        cout << endl; // New line after each vector
    }
}

// Function to determine the file type from its extension
string get_type_from_extension(const string& filename) {
    size_t dotPos = filename.rfind('.');
    if (dotPos == string::npos) {
        cerr << "No extension found, aborting." << endl;
        exit(0);
    }
    string extension = filename.substr(dotPos);
    if (extension == ".fvecs") {
        return "float";
    } else if (extension == ".ivecs") {
        return "int";
    } else {
        cerr << "Unknown type extension, aborting..." << endl;
        exit(0);  // Unsupported extension
    }
}

vector<vector<TypeVariant>> read_from_file(char * file, string &type){
    int fd = open(file, O_RDONLY);
    if (fd == -1) {
        cerr<<"Error with opening file,aborting."<<endl;
        exit(0);
    }
    vector<vector<TypeVariant>> database;
    while(true){
        int d;
        size_t bytes_read;
        bytes_read=read(fd,&d,sizeof(int));
        if(bytes_read==0){//end of file
            break;
        }
        if (type == "int") {
            vector<int> vec(d);
            ssize_t vector_bytes = read(fd, vec.data(), d * sizeof(int));
            if (vector_bytes != d * sizeof(int)){
                cerr << "Error reading int vector data." << endl;
                break;
            }
            vector<TypeVariant> variant_vec(vec.begin(), vec.end()); // Convert int vector to TypeVariant vector
            // Store the vector into the database
            database.push_back(variant_vec);
        }
        else if (type == "float") {
            // cout<<"d: "<<d<<endl;
            vector<float> vec(d);
            ssize_t vector_bytes = read(fd, vec.data(), d * sizeof(float));
            if (vector_bytes != d * sizeof(float)) {
                cerr << "Error reading float vector data." << endl;
                break;
            }
            // Store the vector into the database
            vector<TypeVariant> variant_vec(vec.begin(), vec.end()); // Convert float vector to TypeVariant vector
            database.push_back(variant_vec);        
        }
    }
    close(fd); 
    return database;
}

int main(int argc, char **argv){
    if (argc < 2) {
        cerr<<"Usage: "<<argv[0]<<" <filename>"<< endl;
        return 0;
    }
    //to have 2 decimal points for floats
    cout << fixed << setprecision(3);
    string file_type = get_type_from_extension(argv[1]);
    // Read from the file based on the detected type
    vector<vector<TypeVariant>> database = read_from_file(argv[1],file_type);
    
    // Print the contents of the database
    print_vector(database);
    return 0; // Return success
}
