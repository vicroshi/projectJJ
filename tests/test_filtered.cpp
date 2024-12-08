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
#include <utils.h>

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

void test_stitched() {
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
    std::unordered_map<int,int> S ={{1,2}};
    std::unordered_set<int> L,V_f;
    std::unordered_map<int, std::vector<int>> Pf = {
        {1,{0,1,2}},
        {2,{3,4}},
        {3,{5}}
    };
    V.Pf = Pf;
    TEST_ASSERT(V.vecnum==6);
    TEST_ASSERT(V.db->filters_set->size()==3);

    V.stitched_vamana_indexing(1.1f, 32, 32, 100);
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

    size_t R=3;
    float a=1.1f;
    std::vector<float> vecs_filters = {1.0f,5.0f,2.0f,2.0f,1.0f,3.0f,1.0f,2.0f,3.0f,5.0f,1.0f,2.0f,3.0f,5.0f};
    std::unordered_set<float> filters = {1.0f, 2.0f, 3.0f, 5.0f};
    Matrix<float> f_m(4,14,&vecs, &vecs_filters, &filters);
    VamanaIndex V_F(6,&f_m); //make a random graph with 6 neighbors for each node

    std::unordered_set<int>Lf={},vf={};
    //prune some edges for some vectors
    V_F.filtered_robust_prune(0,vf,a,R);
    TEST_ASSERT(V_F.graph[0].size()<=3);

    //prune some edges for some vectors
    V_F.filtered_robust_prune(5,vf,a,R);
    TEST_ASSERT(V_F.graph[5].size()<=3);



}


void test_filtered_vamana_indexing(){

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

    size_t R=5;
    float a=1.1f;
    size_t list_size=10;
    std::vector<float> vecs_filters = {1.0f,5.0f,2.0f,2.0f,1.0f,3.0f,1.0f,2.0f,3.0f,5.0f,1.0f,2.0f,3.0f,5.0f};
    std::unordered_set<float> filters = {1.0f, 2.0f, 3.0f, 5.0f};
    Matrix<float> f_m(4,14,&vecs, &vecs_filters, &filters);
    VamanaIndex V_F(&f_m); 

    std::unordered_map<float, int> Medoid={
        {1.0f, 0},
        {2.0f, 3},
        {3.0f, 5},
        {5.0f, 13}
    };   

    V_F.filtered_vamana_indexing(Medoid,a,list_size,R);

    //check each node has at most R neighbors
    for(size_t i=0;i<V_F.vecnum;i++){
        TEST_ASSERT(V_F.graph[i].size()<=R);
    }

}