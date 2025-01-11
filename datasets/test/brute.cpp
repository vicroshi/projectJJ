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

//#define BASE_FILE_PATH "../datasets/release1M/contest-data-release-1m.bin"
//#define QUERY_FILE_PATH "../datasets/release1M/contest-queries-release-1m.bin"
//#define GROUND_FILE_PATH "../datasets/release1M/contest-ground-release-1m.bin"
#define BASE_FILE_PATH "../datasets/dummy/dummy-data.bin"
#define QUERY_FILE_PATH "../datasets/dummy/dummy-queries.bin"
#define GROUND_FILE_PATH "../datasets/dummy/dummy-ground.bin"
#include <queue>

#include "omp.h"

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

void WriteBin(const std::string &file_path, const int num_dimensions, const std::vector<std::vector<int>> &data, const uint32_t N) {
    std::ofstream ofs;
    ofs.open(file_path, std::ios::binary);
    assert(ofs.is_open());
    ofs.write((char *)&N, sizeof(uint32_t));
    for (const auto &row : data) {
        if (row.size()) {
            ofs.write((char *)row.data(), num_dimensions * sizeof(int));
        }
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

struct k_max_heap {
    size_t k;
    k_max_heap(size_t k) : k(k) {}
    std::priority_queue<std::pair<int,float>> pq;
    void push(int idx, float dist) {
        if (pq.size() < k) {
            pq.emplace(idx, dist);
        }
        else if (dist < pq.top().second){
            pq.pop();
            pq.emplace(idx, dist);
        }
    }
    int pop() {
        if (!pq.empty()) {
            auto top = pq.top();
            pq.pop();
            return top.first;
        }
        return -1;
    }

};
#define K 100
int main(){
    uint32_t query_no_of_points;
    std::vector<std::vector<float>> query_data;  //only used for reading, will be dropped after data extraction
    ReadBin(QUERY_FILE_PATH,104,query_data,query_no_of_points);
    int counter = 0;
    std::cout << "query_no_of_points:" << query_no_of_points << std::endl;
    for(auto i:query_data){
        if(i[0]<=1.0f){
            counter++;
        }
    }
    std::cout << "counter:" << counter << std::endl;
    std::vector<std::vector<float>> base_data; //only used for reading, will be dropped after data extraction
    uint32_t base_no_of_points;
    ReadBin(BASE_FILE_PATH,102,base_data,base_no_of_points);
        
    std::vector<std::vector<int>> neighbors(base_no_of_points); // Initialize with empty vectors

    size_t dim = base_data[0].size() - 2;
    #pragma omp parallel for num_threads(16)
    for (size_t i = 0; i < query_no_of_points; i++) { // For each query point
//        std::cout << "query " << i << std::endl;
        k_max_heap kheap(K);
        std::span query_span(query_data[i].data()+4, query_data[i].size());
        for (size_t j = 0; j < base_no_of_points; j++) {
            std::span base_span(base_data[j].data()+2, base_data[j].size());
            if (query_data[i][0] == 1.0f) {
                if (base_data[j][0] == query_data[i][1]) {
                    float dist = sq_euclid(query_span, base_span, dim);
                    kheap.push(j,dist);
                }
            }
            else if (query_data[i][0] == 0.0f) {
                float dist = sq_euclid(query_span, base_span, dim);
                kheap.push(j,dist);
            }
        }
        if (query_data[i][0] <= 1.0f){
            neighbors[i].resize(K,-1);
            for (int j = kheap.pq.size()-1; j >= 0; j--) {
                neighbors[i][j] = kheap.pop();
            }
        }
    }
    WriteBin(GROUND_FILE_PATH, K, neighbors, counter);
    return 0;
}
