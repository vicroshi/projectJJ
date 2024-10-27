#define TEST_NO_MAIN

#include <iostream>
#include <acutest.h>
#include <span>
#include <vector>
#include "database.h"
#include "graph.h"
#include <set>
void test_greedy(){
    // int p[5][2]={
    //     {4,1},
    //     {10,0},
    //     {7,3},
    //     {2,2},
    //     {9,8}
    // };

    
}

void test_k_closest(){
    int points_i[6][4] = {
        {0, 1, 2, 3},
        {4, 5, 6, 7},
        {8, 9, 10, 11},
        {12, 13, 14, 15},
        {0, 1, 4, 6},  
        {8, 3, 15, 1}  
    };
    Matrix<int> i_m(4,6,&points_i[0][0]);
    VamanaIndex<int> VI(&i_m);
    VI.vecnum=6;
    std::vector<int> int_q ={1,2,5,10};
    const std::span<int> q_i(int_q.data(), int_q.size());

    std::set<int> L={0,1,2,3,4,5};
    VI.keep_k_closest(L,4,q_i);
    std::set<int> true_closest_idx={1,4,0,2};
    
    //sort to be able to compare them
    // std::sort(true_closest_idx.begin(), true_closest_idx.end());
    // std::sort(L.begin(), L.end());
    //check if L has the true_closest_idx in it
    TEST_ASSERT(true_closest_idx==L);



    //for floats
    float points_f[6][4] = {
        {0.1f, 1.5f, 2.0f, 3.0f},
        {4.0f, 5.0f, 6.0f, 7.0f},
        {8.0f, 9.0f, 10.0f, 11.0f},
        {12.0f, 13.0f, 14.0f, 15.0f},
        {0.0f, 1.0f, 4.0f, 6.0f},  
        {8.0f, 3.0f, 15.0f, 1.0f}  
    };
    Matrix<float> f_m(4,6,&points_f[0][0]);
    VamanaIndex<float> VF(&f_m);
    VF.vecnum=6;
    std::vector<float> float_q ={1.0f,2.0f,5.0f,10.0f};
    const std::span<float> q_f(float_q.data(), float_q.size());

    L={0,1,2,3,4,5};
    VF.keep_k_closest(L,2,q_f);
    true_closest_idx={1,4};

    TEST_ASSERT(true_closest_idx==L);
}