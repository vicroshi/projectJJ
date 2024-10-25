#include <iostream>
#include <string>
#include <variant>
#include <limits>
//check the type and return the data stored
#define EXTRACT_FROM_VARIANT(var)                       \
    (var.index() == 0 ? *std::get_if<int>(&var) :       \
    (var.index() == 1 ? *std::get_if<float>(&var) :     \
    (throw std::runtime_error("Error: Invalid type"))))

int main1(){
    //works
    // std::variant<int, float> a; 
    // a=17;
    // // std::cout<<i<<std::endl;
    // std::cout<<EXTRACT_FROM_VARIANT(a)<<std::endl;
    // a=12.324f;
    // std::cout<<EXTRACT_FROM_VARIANT(a)<<std::endl;


    std::string base_file,query_file;
    int k,L,a;
    while(true){
        std::cout<<"base file path (Ctrl-C to stop): ";
        std::cin>>base_file;
        std::cout<<"you provided: "<< base_file<<std::endl;

        //TO_DO: read here and give the range of the query point [0,n-1]

        std::cout<<"query file path: ";
        std::cin>>query_file;
        std::cout<<"you provided: "<< query_file<<std::endl;
        std::cout<<"alpha: ";
        std::cin>>a;
        // std::cout<<"you provided: "<< a<<std::endl;
         // Check if the input failed
        if (std::cin.fail()) {
            std::cout << "Invalid input. Please enter a  integer." << std::endl;
            exit(1);
        }
        std::cout<<"k: ";
        std::cin>>k;
        // std::cout<<"you provided: "<< k<<std::endl;
         // Check if the input failed
        if (std::cin.fail()) {
            std::cout << "Invalid input. Please enter a  integer." << std::endl;
            exit(1);
        }
        std::cout<<"List size (>=k): ";
        std::cin>>L;
        // std::cout<<"you provided: "<< L<<std::endl;
         // Check if the input failed
        if (std::cin.fail()) {
            std::cout << "Invalid input. Please enter a  integer." << std::endl;
            exit(1);
        }


        
    }
    return 0;
}


template <typename T>
T* read_from_file(T arg) {
    
    // Simulate reading logic based on type T
    if constexpr(std::is_same_v<T, int>) {
        std::cout<<"value="<<arg<<std::endl;    
        //READ from file and add it to a int* array
    } else if constexpr (std::is_same_v<T, float>) {
        std::cout<<"value="<<arg<<std::endl;
        //read from file and add it to a float* array
    }
}

template <typename T>
void execute(T arg){
    size_t base_dim,query_dim,base_vecs_num,query_vecs_num;
    T* base=read_from_file<T>(arg,&base_dim,&base_vecs_num);

    // std::cout<<value<<std::endl;

}
// apo edw orizw ton typo pou tha checkarei i read
int main(){
    execute<int>(4);
    execute<float>(4.12);
    return 0;
}