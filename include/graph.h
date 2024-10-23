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
//template <typename T>
struct VamanaIndex {
    std::vector<std::unordered_set<int>> graph;
//    std::span<T> db;
    int vecnum;
    //methods
    VamanaIndex(int vecnum, int deg);
    void init_graph(int r);
    void print_graph();
};
#endif //PROJECTJJ_GRAPH_H
