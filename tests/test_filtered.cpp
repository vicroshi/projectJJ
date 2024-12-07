//
// Created by vic on 06/12/2024.
//
#define TEST_NO_MAIN

#include <iostream>
#include <acutest.h>
#include <span>
#include <vector>
#include "database.h"
#include "graph.h"
#include <set>
#include <unordered_set>

void test_filtered_greedy(){
    std::cout<<std::endl;
    int points[6][4] = {
        {3, 7, 12, 8},
        {10, 2, 6, 14},
        {5, 15, 1, 9},
        {0, 13, 4, 11},
        {7, 3, 10, 2},
        {12, 5, 8, 6}
    };
    std::vector vecs(&points[0][0], &points[0][0] + 6*4);
    std::vector vecs_filters = {1,1,1,2,2,3};
    std::unordered_set filters = {1,2,3};
    int q[] = {0,0,0,0};
    std::span query(q,4);
    int fq = 2;
    Matrix<int> i_m(4,6,&vecs, &vecs_filters, &filters);
    VamanaIndex<int> V(&i_m);
    V.graph[0].insert(1);
    V.graph[0].insert(2);
    V.graph[1].insert(0);
    V.graph[1].insert(2);
    V.graph[2].insert(0);
    V.graph[3].insert(4);
    V.graph[4].insert(3);
    std::unordered_map<int,int> S ={{2,3}};
    std::unordered_set<int> L,V_f;
    V.filtered_greedy_search(S,query,1,5,fq,L,V_f);
    TEST_ASSERT(L.size()==1);
    TEST_ASSERT(L.contains(4));
    V.filtered_greedy_search(S,query,2,5,4,L,V_f);
    TEST_ASSERT(L.empty());


}

void test_filtered_prune() {

    float points_f[14][4] = {
    {3.0f, 7.0f, 12.0f, 8.0f},
    {5.0f, 15.0f, 1.0f, 9.0f},
    {0.0f, 13.0f, 4.0f, 11.0f},
    {10.0f, 2.0f, 6.0f, 14.0f},
    {12.0f, 5.0f, 8.0f, 6.0f},
    {9.0f, 0.0f, 7.0f, 13.0f},
    {14.0f, 2.0f, 11.0f, 3.0f},
    {4.0f, 8.0f, 15.0f, 5.0f},
    {6.0f, 10.0f, 3.0f, 12.0f},
    {1.0f, 9.0f, 14.0f, 0.0f},
    {11.0f, 4.0f, 13.0f, 7.0f},
    {2.0f, 6.0f, 9.0f, 15.0f},
    {15.0f, 1.0f, 5.0f, 10.0f},
    {8.0f, 12.0f, 2.0f, 4.0f}
    };
    std::vector<float> vecs(&points_f[0][0], &points_f[0][0] + 14*4);

    size_t R=2;
    float a=1.1;
    size_t t=10;
    size_t list_size=10;
    std::vector<float> vecs_filters = {1.0f,5.0f,2.0f,2.0f,1.0f,3.0f,1.0f,2.0f,3.0f,5.0f,1.0f,2.0f,3.0f,5.0f};
    std::unordered_set<float> filters = {1.0f, 2.0f, 3.0f, 5.0f};
    Matrix<float> f_m(4,14,&vecs, &vecs_filters, &filters);
    VamanaIndex V_F(&f_m);

    std::unordered_map<float, int> Medoid;          
    std::unordered_map<float, std::vector<int>> Pf;
    f_m.find_medoid(t, Medoid, Pf);

    std::cout<<"before indexing:\n";
    V_F.print_graph();
    V_F.filtered_vamana_indexing(Medoid,a,list_size,R);
    std::cout<<"after indexing:\n";
    V_F.print_graph();
    
    //graph must be of at most R-out degree
    for(size_t i=0;i<V_F.graph.size();i++){
        TEST_ASSERT(V_F.graph[i].size()<=R);
    }


}
