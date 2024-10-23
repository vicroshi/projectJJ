//
// Created by vic on 17/10/2024.
//

#include "graph.h"
#include <iomanip>
VamanaIndex::VamanaIndex(int vecnum, int deg) {
    this->vecnum = vecnum;
    init_graph(deg);
}
void VamanaIndex::init_graph(int r) {
    graph.resize(r);
    int idx = 0;
    const auto range = std::ranges::iota_view(0, vecnum);
    std::vector<int> idcs(range.begin(), range.end());
    std::vector<int> samp;
    for (auto &set : graph) {
        samp.clear();
        set.insert(idx);
        std::sample(idcs.begin(),idcs.end(), back_inserter(samp), r+1, std::mt19937{std::random_device{}()});
        set.insert(samp.begin(), samp.end());
        set.erase(idx);
        if (set.size() > r) {
            set.erase(set.begin());
        }
        idx++;
    }
}

void VamanaIndex::print_graph(){
    for (auto &set : graph) {
        for (auto &i : set) {
            std::cout <<std::setw(3) << i << " ";
        }
        std::cout << std::endl;
    }
}