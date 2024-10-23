#include <iostream>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>
#include <filesystem>
#include "graph.h"
#include "database.h"
#include <random>
#include <sys/stat.h>
#include <iomanip> // Include iomanip for setprecision


int main() {
    std::vector<float> vec;
//    std::string name = "../datasets/siftsmall/siftsmall_base.fvecs";
    std::string dbname = "../datasets/siftsmall/siftsmall_base.fvecs";
    std::string qname = "../datasets/siftsmall/siftsmall_query.fvecs";

    size_t d1,d2,n1,n2;
    float* base = read_fvecs(dbname, &d1, &n1);
    std::span<float> db(base, d1*n1);
    float* query = read_fvecs(qname, &d2, &n2);
    std::span<float> qr(query, d2*n2);
    int i = 0;
    int j = 0;
    bool flag;
    for (int q_idx = 0; q_idx < n2; q_idx++) {
        for (int v_idx = 0; v_idx < n1; v_idx++) {
            flag = true;
//            std::cout << "Vector " << v_idx << std::endl;
            i++;
            for (int d_idx = 0; d_idx < d1; d_idx++) {
                if (qr[q_idx*d1+d_idx] != db[v_idx*d1+d_idx]) {
                    flag = false;
                }
//                    std::cout << std::fixed << std::setprecision(1) << std::setw(6) << qr[j] << " ";
//                }
//                std::cout << std::fixed << std::setprecision(1) <<std::setw(6) << db[i] << " ";
//                i++;
            }
            if (flag) {
                std::cout << "Match found at Vector#" << v_idx << "and Query#" <<q_idx << std::endl;
            }
//            j++;
//            std::cout << std::endl;
        }
    }
//    std::cout << "Total iterations: " << i << std::endl;

    return 0;
}

