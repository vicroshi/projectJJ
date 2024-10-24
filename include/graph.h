//
// Created by vic on 17/10/2024.
//

#ifndef PROJECTJJ_GRAPH_H
#define PROJECTJJ_GRAPH_H

#include <map>
#include <list>
#include <iostream>
#include <vector>
#include <algorithm>
#include <unordered_set>
#include <ranges>
#include <random>
#include <span>
#include <iomanip>
#include <database.h>
template <typename T>
struct VamanaIndex {
    std::vector<std::unordered_set<int>> graph;
    Matrix<T>* db;
    int vecnum;
    VamanaIndex(int deg, T* db):db(db) {
        this->vecnum = db->vecnum;
        init_graph(deg);

    }

    void init_graph(int r){
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

    void print_graph(){
        for (auto &set : graph) {
            for (auto &i : set) {
                std::cout <<std::setw(3) << i << " ";
            }
            std::cout << std::endl;
        }
    }

    void greedy_search() {
    }

};


#endif //PROJECTJJ_GRAPH_H
