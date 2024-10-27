#define TEST_NO_MAIN

#include <iostream>
#include <acutest.h>
#include <span>
#include <vector>
#include "database.h"
#include "graph.h"
#include <set>
void test_greedy(){
    std::cout<<std::endl;
    int points[6][4] = {
        {3, 7, 12, 8}, //78
        {10, 2, 6, 14}, //118
        {5, 15, 1, 9}, //202
        {0, 13, 4, 11}, //132
        {7, 3, 10, 2}, //98
        {12, 5, 8, 6} //143
    };
    //distances are squared here!
// Point [0]: 78
// Point [1]: 118
// Point [2]: 202
// Point [3]: 132
// Point [4]: 98  
// Point [5]: 143

//in order:{0,4,1,3,5,2}

    Matrix<int> i_m(4,6,&points[0][0]);
    VamanaIndex<int> V(3,&i_m);
    // V.print_graph();
    std::set<int>L={},v={};
    std::vector<int> int_q ={1,2,5,8};
    const std::span<int> q_i(int_q.data(), int_q.size());
    int start_ind=i_m.medoid_naive();
    std::cout<<"medoid: "<<start_ind<<std::endl;
    //for k=1:
    V.greedy_search(start_ind,q_i,1,5,L,v);
    TEST_ASSERT(L.size()==1);
    std::set<int>sol={0};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //for k=2:
    V.greedy_search(start_ind,q_i,2,5,L,v);
    TEST_ASSERT(L.size()==2);
    sol={0,4};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;     TEST_ASSERT(L==sol);
    //for k=3:
    V.greedy_search(start_ind,q_i,3,5,L,v);
    TEST_ASSERT(L.size()==3);
    sol={0,4,1};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;     TEST_ASSERT(L==sol);
    //for k=4:
    V.greedy_search(start_ind,q_i,4,5,L,v);
    TEST_ASSERT(L.size()==4);   
    sol={0,4,1,3};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //for k=5:
    V.greedy_search(start_ind,q_i,5,5,L,v);
    TEST_ASSERT(L.size()==5);   
    sol={0,4,1,3,5};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    
    //floats
    std::cout<<std::endl;
    float points_f[6][4] = {
        {3.2, 12.6, 12.0, 8.5},
        {10.0, 2.5, 6.0, 14.0},
        {5.0, 15.7, 1.0, 9.0},
        {0.0, 13.0, 4.1, 11.8},
        {7.0, 3.0, 10.2, 2.0},
        {12.1, 5.0, 8.4, 6.0}
    };
//actual distances
// Point [0]: 12.53
// Point [1]: 10.53
// Point [2]: 14.42
// Point [3]: 11.26
// Point [4]: 10.04  
// Point [5]: 11.93

//in order:{4,1,3,5,0,2}

    Matrix<float> f_m(4,6,&points_f[0][0]);
    VamanaIndex<float> Vf(3,&f_m);
    // V.print_graph();
    L={};
    v={};
    std::vector<float> float_q ={1.2,2.4,5.0,8.3};
    const std::span<float> q_f(float_q.data(), float_q.size());
    start_ind=i_m.medoid_naive();
    std::cout<<"medoid: "<<start_ind<<std::endl;
    //for k=1:
    Vf.greedy_search(start_ind,q_f,1,5,L,v);
    TEST_ASSERT(L.size()==1);
    sol={4};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl; 
    TEST_ASSERT(L==sol);
    //for k=2:
    Vf.greedy_search(start_ind,q_f,2,5,L,v);
    TEST_ASSERT(L.size()==2);
    sol={4,1};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //for k=3:
    Vf.greedy_search(start_ind,q_f,3,5,L,v);
    TEST_ASSERT(L.size()==3);
    sol={4,1,3};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //for k=4:
    Vf.greedy_search(start_ind,q_f,4,5,L,v);
    TEST_ASSERT(L.size()==4);
    sol={4,1,3,5};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //k==5
    Vf.greedy_search(start_ind,q_f,5,5,L,v);
    TEST_ASSERT(L.size()==5);
    sol={4,1,3,5,0};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
}

void test_k_closest(){
    int points_i[6][3] = {
        {0, 1, 30},
        {4, 5, 6},
        {8, 9, 10},
        {12, 13, 14},
        {0, 1, 4},  
        {8, 3, 15}  
    };
    Matrix<int> i_m(3,6,&points_i[0][0]);
    VamanaIndex<int> VI(&i_m);
    VI.vecnum=6;
    std::vector<int> int_q ={1,2,5};
    const std::span<int> q_i(int_q.data(), int_q.size());

    std::set<int> L={0,1,2,3,4,5};
    VI.keep_k_closest(L,2,q_i);
    std::set<int> true_closest_idx={1,4};

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

void test_prune(){
    std::cout<<std::endl;
    int points[15][4] = {
    {3, 7, 12, 8},
    {10, 2, 6, 14},
    {5, 15, 1, 9},
    {0, 13, 4, 11},
    {7, 3, 10, 2},
    {12, 5, 8, 6},
    {9, 0, 7, 13},
    {14, 2, 11, 3},
    {4, 8, 15, 5},
    {6, 10, 3, 12},
    {1, 9, 14, 0},
    {11, 4, 13, 7},
    {2, 6, 9, 15},
    {15, 1, 5, 10},
    {8, 12, 2, 4}
    };

    Matrix<int> i_m(4,15,&points[0][0]);
    VamanaIndex<int> V(15,&i_m);
    std::cout<<"before pruning: "<<std::endl;
    V.print_graph();
    float a=2.5f;
    std::set<int>L={},v={};
    int medoid=i_m.medoid_naive();
    V.robust_prune(0,v,a,10);
    // V.robust_prune(0,v,a,10);
    V.robust_prune(3,v,a,10);
    std::cout<<"\nafter pruning: "<<std::endl;

    V.print_graph();
}