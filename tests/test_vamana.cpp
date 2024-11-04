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
        {3, 7, 12, 8}, 
        {10, 2, 6, 14},
        {5, 15, 1, 9},
        {0, 13, 4, 11}, 
        {7, 3, 10, 2},
        {12, 5, 8, 6} 
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
    VamanaIndex<int> V(&i_m);
    V.graph[0].insert(1);
    V.graph[0].insert(2);
    V.graph[0].insert(3);
    V.graph[0].insert(5);
    V.graph[0].insert(4);

    V.graph[1].insert(5);
    V.graph[1].insert(4);
    V.graph[1].insert(3);
    V.graph[1].insert(2);
    V.graph[1].insert(0);

    V.graph[2].insert(3);
    V.graph[2].insert(4);
    V.graph[2].insert(0);
    V.graph[2].insert(5);
    V.graph[2].insert(1);

    V.graph[3].insert(4);
    V.graph[3].insert(5);
    V.graph[3].insert(1);
    V.graph[3].insert(2);
    V.graph[3].insert(0);

    V.graph[4].insert(2);
    V.graph[4].insert(1);
    V.graph[4].insert(3);
    V.graph[4].insert(0);
    V.graph[4].insert(5);

    V.graph[5].insert(0);
    V.graph[5].insert(3);
    V.graph[5].insert(4);
    V.graph[5].insert(1);
    V.graph[5].insert(2);
    V.print_graph();
    std::unordered_set<int>L={},v={};
    std::vector<int> int_q ={1,2,5,8};
    const std::span<int> q_i(int_q.data(), int_q.size());
    // int start_ind=i_m.medoid_naive();
    int start_ind=0;
    std::cout<<"medoid: "<<start_ind<<std::endl;
    //for k=1:
    V.greedy_search(start_ind,q_i,1,5,L,v);
    TEST_ASSERT(L.size()==1);
    std::unordered_set<int>sol={0};
    TEST_ASSERT(L==sol);
    //for k=2:
    L.clear();
    v.clear();
    V.greedy_search(start_ind,q_i,2,5,L,v);
    TEST_ASSERT(L.size()==2);
    sol={0,4};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;     
    TEST_ASSERT(L==sol);
    //for k=3:
    L.clear();
    v.clear();
    V.greedy_search(start_ind,q_i,3,5,L,v);
    TEST_ASSERT(L.size()==3);
    sol={0,4,1};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;     
    TEST_ASSERT(L==sol);
    //for k=4:
    L.clear();
    v.clear();
    V.greedy_search(start_ind,q_i,4,5,L,v);
    TEST_ASSERT(L.size()==4);   
    sol={0,4,1,3};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //for k=5:
    L.clear();
    v.clear();
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
        {3.0, 7.0, 12.0, 8.0}, //78
        {10.0, 2.0, 6.0, 14.0}, //118
        {5.0, 15.0, 1.0, 9.0}, //202
        {0.0, 13.0, 4.0, 11.0}, //132
        {7.0, 3.0, 10.0, 2.0}, //98
        {12.0, 5.0, 8.0, 6.0} //143
    };
    //distances are squared here!
    // Point [0]: 78
    // Point [1]: 118
    // Point [2]: 202
    // Point [3]: 132
    // Point [4]: 98  
    // Point [5]: 143
    //in order:{0,4,1,3,5,2}

    Matrix<float> f_m(4,6,&points_f[0][0]);
    VamanaIndex<float> Vf(&f_m);
    Vf.graph[0].insert(4);
    Vf.graph[0].insert(5);
    Vf.graph[0].insert(3);
    Vf.graph[0].insert(2);
    Vf.graph[0].insert(1);

    Vf.graph[1].insert(5);
    Vf.graph[1].insert(4);
    Vf.graph[1].insert(3);
    Vf.graph[1].insert(2);
    Vf.graph[1].insert(0);

    Vf.graph[2].insert(3);
    Vf.graph[2].insert(4);
    Vf.graph[2].insert(0);
    Vf.graph[2].insert(5);
    Vf.graph[2].insert(1);

    Vf.graph[3].insert(4);
    Vf.graph[3].insert(5);
    Vf.graph[3].insert(1);
    Vf.graph[3].insert(2);
    Vf.graph[3].insert(0);

    Vf.graph[4].insert(2);
    Vf.graph[4].insert(1);
    Vf.graph[4].insert(3);
    Vf.graph[4].insert(0);
    Vf.graph[4].insert(5);

    Vf.graph[5].insert(0);
    Vf.graph[5].insert(3);
    Vf.graph[5].insert(4);
    Vf.graph[5].insert(1);
    Vf.graph[5].insert(2);
    // V.print_graph();
    L={};
    v={};
    std::vector<float> float_q ={1.0,2.0,5.0,8.0};
    const std::span<float> q_f(float_q.data(), float_q.size());
    // start_ind=i_m.medoid_naive();
    std::cout<<"medoid: "<<start_ind<<std::endl;
    //for k=1:
    L.clear();
    v.clear();
    Vf.greedy_search(start_ind,q_f,1,5,L,v);
    TEST_ASSERT(L.size()==1);
    sol={0};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl; 
    TEST_ASSERT(L==sol);
    //for k=2:
    L.clear();
    v.clear();
    Vf.greedy_search(start_ind,q_f,2,5,L,v);
    TEST_ASSERT(L.size()==2);
    sol={0,4};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //for k=3:
    L.clear();
    v.clear();
    Vf.greedy_search(start_ind,q_f,3,5,L,v);
    TEST_ASSERT(L.size()==3);
    sol={4,1,0};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //for k=4:
    L.clear();
    v.clear();
    Vf.greedy_search(start_ind,q_f,4,5,L,v);
    TEST_ASSERT(L.size()==4);
    sol={4,1,3,0};
    for(auto item:sol) std::cout<<item<<",";
    std::cout<<std::endl;
    for(auto item:L) std::cout<<item<<","; 
    std::cout<<std::endl;std::cout<<std::endl;
    TEST_ASSERT(L==sol);
    //k==5
    L.clear();
    v.clear();
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

    std::unordered_set<int> L={0,1,2,3,4,5};
    VI.keep_k_closest(L,2,q_i);
    std::unordered_set<int> true_closest_idx={1,4};

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
    VamanaIndex<int> V(7,&i_m);
    std::cout<<"before pruning: "<<std::endl;
    V.print_graph();
    float a=2.4f;
    std::unordered_set<int>L={},v={};
    size_t R=5;
    V.robust_prune(0,v,a,R);
    TEST_ASSERT(V.graph[0].size()<=R); //node should now have at most R neighbors
    V.robust_prune(5,v,a,R);
    TEST_ASSERT(V.graph[5].size()<=R); //node should now have at most R neighbors
    std::cout<<"\nafter pruning: "<<std::endl;
    V.print_graph();

    //for floats too
    std::cout<<"/////////\n";
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

    Matrix<float> f_m(4,14,&points_f[0][0]);
    VamanaIndex<float> V_F(7,&f_m);
    std::cout<<"before pruning:"<<std::endl;
    V_F.print_graph();
    a=1.0f;
    std::unordered_set<int>Lf={},vf={};
    R=5;
    V_F.robust_prune(0,vf,a,R);
    V_F.robust_prune(3,vf,a,R);
    std::cout<<"\nafter pruning:\n"<<std::endl;
    V_F.print_graph();
    TEST_ASSERT(V_F.graph[0].size()<=R); //node should now have at most R neighbors
    TEST_ASSERT(V_F.graph[3].size()<=R); //node should now have at most R neighbors
    
}


void test_vamana_index(){
    //ints
    std::cout<<std::endl;
    int points[14][4] = {
    {3, 7, 12, 8},
    {5, 15, 1, 9},
    {0, 13, 4, 11},
    {10, 2, 6, 14},
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
    int R=7;
    float a=1.1;
    size_t list_size=10;
    std::set<int>closest={4, 10, 6, 7, 0};
    int q[4]={7,3,10,2};
    std::span<int> q_span(q,4);
    Matrix<int> i_m(4,14,&points[0][0]);
    VamanaIndex<int> V(R,&i_m);
    auto start_ind=i_m.medoid_naive();
    std::cout<<"medoid: "<<start_ind<<std::endl;
    std::cout<<"before indexing:\n";
    V.print_graph();
    V.vamana_indexing(start_ind,a,list_size,R);
    std::cout<<"after indexing:\n";
    V.print_graph();
    //graph must be of at most R-out degree
    for(int i=0;i<V.vecnum;i++){
        TEST_ASSERT(V.graph[i].size()<=7);
    }

    //floats
    std::cout<<std::endl;
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
    R=7;
    a=1.1;
    list_size=10;
    Matrix<float> f_m(4,14,&points_f[0][0]);
    VamanaIndex<float> V_F(R,&f_m);
    start_ind=f_m.medoid_naive();
    std::cout<<"medoid: "<<start_ind<<std::endl;
    std::cout<<"before indexing:\n";
    V_F.print_graph();
    V_F.vamana_indexing(start_ind,a,list_size,R);
    std::cout<<"after indexing:\n";
    V_F.print_graph();
    //graph must be of at most R-out degree
    for(int i=0;i<V_F.vecnum;i++){
        TEST_ASSERT(V_F.graph[i].size()<=7);
    }

}