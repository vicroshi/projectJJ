#include <iostream>
#include <string>
#include <limits>
#include <iterator> // for std::back_inserter
#include "assert.h"
#include <fstream>
#include <numeric>
#include <vector>
#include <span>
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
#include <immintrin.h>
#include <cstddef>
#include <set>
#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <random>

#define BASE_FILE_PATH "../2/dummy-data.bin"
#define QUERY_FILE_PATH "../2/dummy-queries.bin"
#define GROUND_FILE_PATH "ground.bin"


//from sigmod repo
void ReadBin(const std::string &file_path,const int num_dimensions,std::vector<std::vector<float>> &data, uint32_t& N) {
    // std::cout << "Reading Data: " << file_path << std::endl;
    std::ifstream ifs;
    ifs.open(file_path, std::ios::binary);
    assert(ifs.is_open());
    // uint32_t N;  // num of points
    ifs.read((char *)&N, sizeof(uint32_t));
    data.resize(N);
    // std::cout << "# of points: " << N << std::endl;
    std::vector<float> buff(num_dimensions);
    int counter = 0;
    while (ifs.read((char *)buff.data(), num_dimensions * sizeof(float))) {
        std::vector<float> row(num_dimensions);
        for (int d = 0; d < num_dimensions; d++) {
        row[d] = static_cast<float>(buff[d]);
        }
        data[counter++] = std::move(row);
    }
    ifs.close();
    // std::cout << "Finish Reading Data" << std::endl;
}

void WriteBin(const std::string &file_path, const int num_dimensions, const std::vector<std::vector<float>> &data, const uint32_t N) {
    std::ofstream ofs;
    ofs.open(file_path, std::ios::binary);
    assert(ofs.is_open());
    ofs.write((char *)&N, sizeof(uint32_t));
    for (const auto &row : data) {
        ofs.write((char *)row.data(), num_dimensions * sizeof(float));
    }
    ofs.close();
}

// using SIMD instructions to calculate squared distance faster, AVX/AVX2 required
static double sq_euclid(const std::span<float>& row1, const std::span<float>& row2, const size_t& dim) {
    // for floats
    __m256 sum = _mm256_setzero_ps(); // Initialize sum to 0
    size_t i = 0;
    // Process 8 floats at a time
    for (; i + 8 <= dim; i += 8) {
        __m256 a_vec = _mm256_loadu_ps(row1.data() + i);
        __m256 b_vec = _mm256_loadu_ps(row2.data() + i);
        __m256 diff = _mm256_sub_ps(a_vec, b_vec);
        sum = _mm256_fmadd_ps(diff, diff, sum); // sum += diff * diff
    }
    // Sum the results
    float temp[8];
    _mm256_storeu_ps(temp, sum);
    double result = 0.0;
    for (int j = 0; j < 8; ++j) {
        result += temp[j];
    }
    // Process any remaining elements
    for (; i < dim; ++i) {
        float diff = row1.data()[i] - row2.data()[i];
        result += diff * diff;
    }
    return result;
}


int main(){
    uint32_t query_no_of_points;
    std::vector<std::vector<float>> query_data;  //only used for reading, will be dropped after data extraction
    ReadBin(BASE_FILE_PATH,104,query_data,query_no_of_points);
    int counter = 0;
    for(auto i:query_data){
        if(i[0]<=1.0f){
            counter++;
        }
    }
    std::cout << "counter:" << counter << std::endl;
    std::vector<std::vector<float>> base_data; //only used for reading, will be dropped after data extraction
    uint32_t base_no_of_points;
    ReadBin(QUERY_FILE_PATH,102,base_data,base_no_of_points);
        
    std::vector<std::vector<int>> neighbors(counter); // Initialize with empty vectors

    int idx=0;

    for (size_t i = 0; i < query_no_of_points; i++) { // For each query point
        if(query_data[i][0] > 1.0f) continue; // only first two types
            std::vector<int> added(base_data.size(), 0); // Track visited points
            std::span<float> vec1(query_data[i].data() + 4, 100); // Extract query vector

            for (int k = 0; k < 100; k++) { // Find 100 neighbors
                float min = std::numeric_limits<float>::max(); // Reset min for each neighbor
                int min_idx = -1;

                for (size_t j = 0; j < base_no_of_points; j++) { // Search all base points
                    if (added[j] == 1) continue; // Skip already visited points

                    std::span<float> vec2(base_data[j].data() + 2, 100); // Extract base vector
                    float dist = sq_euclid(vec1, vec2, 100);
                    if(query_data[i][0] == 1.0f){
                        // Check both distance and matching condition
                        if (dist < min && base_data[j][0] == query_data[i][1]) {
                            min = dist;
                            min_idx = j;
                        }
                    }
                    else if(query_data[i][0] == 0.0f){
                        // Check both distance 
                        if (dist < min) {
                            min = dist;
                            min_idx = j;
                        }
                    }
                }

                if (min_idx != -1) { // Ensure a valid neighbor was found
                    neighbors[idx].push_back(min_idx);
                    added[min_idx] = 1; // Mark as added
                } else {
                    break; // Exit if no more valid neighbors can be found
                }
            }
            idx++;
            
    }

    for(int i = 0; i < counter; i++){
        if(query_data[i][0] > 1.0f) continue; // filtered
            std::cout << "for filtered query:" << i << " with query type:"<<query_data[i][0] <<",filter:" << query_data[i][1] << " ," << neighbors[i].size() << " nearest neighbors:\n[ ";
            for (auto it = neighbors[i].begin(); it != neighbors[i].end(); ++it) {
                     std::cout << *it << " ";
            }
            // std::cout<<"\n";
            std::cout << "]\n\n";
            // while(neighbors[i].size() < 100){
            //     neighbors[i].insert(-static_cast<int>(neighbors[i].size()) - 1);
            // }
            
            
        
    }

    std::vector<std::vector<float>> neighbors_float(counter, std::vector<float>(100, -1.0f));

    for (int i = 0; i < counter; i++) {
            int index = 0;
            for (const auto& neighbor : neighbors[i]) {
                neighbors_float[i][index++] = static_cast<float>(neighbor);
            }
    }

    WriteBin(GROUND_FILE_PATH, 100, neighbors_float, counter);

    return 0;
}
