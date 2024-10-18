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
using namespace std;
class VamanaIndex {
    private:
        vector<unordered_set<int>> graph;
        int vecnum;
        void init_graph(int r);
    public:
        VamanaIndex(int vecnum, int deg);
        void print_graph();
};
#endif //PROJECTJJ_GRAPH_H
