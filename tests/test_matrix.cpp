//
// Created by vic on 25/10/2024.
//
#define TEST_NO_MAIN

#include <acutest.h>
#include <database.h>
void test_constructor(){
    //dokimazw oles tis periptwseis tou T, eite int eite float
    float* p = new float[100];
    for (int i = 0; i < 100; i++) {
        p[i] = static_cast<float>(i) + 0.2;
    }
    Matrix<float> m(10, 10, p);
                                //alliws 0.2f
    TEST_ASSERT(m.get(0, 0) == (float)0.2);
    TEST_ASSERT(m.get(1, 1) == (float)11.2);
    TEST_ASSERT(m.dim == 10);
    TEST_ASSERT(m.vecnum == 10);
    delete[] p;
    //case its int
    int *x = new int[100];
    for (int i = 0; i < 100; i++) {
        x[i] = i;
    }
    Matrix<int> n(10, 10, x);
    TEST_ASSERT(n.get(0, 0) == 0);
    TEST_ASSERT(n.get(1, 1) == 11);
    TEST_ASSERT(n.dim == 10);
    TEST_ASSERT(n.vecnum == 10);
    delete[] x;
}

void test_euclid() {
    float p[4][4] = {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {1, 3, 10, 2},
        {2, 3, 5, 7}
    };
    Matrix<float> m(4,4,&p[0][0]);
    std::cout<<"m0:";
    for(const auto &i: m.row(0)){
        std::cout<< i<<std::endl;
    }
    std::cout<<"m1:";
    for(const auto &i: m.row(1)){
        std::cout<< i<<std::endl;
    }

    // std::cout << m.row(0) <<","<< m.row(1) << std::endl;
    std::cout<<"dist: "<<m.sq_euclid(m.row(0), m.row(1),m.row(0).size())<<std::endl;
    TEST_ASSERT(m.sq_euclid(m.row(0), m.row(1),m.row(0).size()) == 4.0);
    TEST_ASSERT(m.sq_euclid(m.row(0), m.row(2),m.row(0).size()) == 114.0);
    TEST_ASSERT(m.sq_euclid(m.row(0), m.row(3),m.row(0).size()) == 87.0);
    TEST_ASSERT(m.sq_euclid(m.row(2), m.row(3),m.row(0).size()) == 51.0);
    int i[4][4] = {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {1, 3, 10, 2},
        {2, 3, 5, 7}
    };
    Matrix<int> n(4,4,&i[0][0]);
    // Matrix<int> n(4,4,reinterpret_cast<int*>(&p[0][0]));
    
    std::cout << n.sq_euclid(n.row(0), n.row(1),n.row(0).size()) << std::endl;
    TEST_ASSERT(n.sq_euclid(n.row(0), n.row(1),n.row(0).size()) == 4.0);
    TEST_ASSERT(n.sq_euclid(n.row(0), n.row(2),n.row(0).size()) == 114.0);
    TEST_ASSERT(n.sq_euclid(n.row(0), n.row(3),n.row(0).size()) == 87.0);
    TEST_ASSERT(n.sq_euclid(n.row(2), n.row(3),n.row(0).size()) == 51.0);
}

void test_medoid() {
    int p[8][4] = {
        {1, 2, 3, 4},
        {4, 5, 6, 7},
        {1, 1, 1, 1},
        {2, 3, 5, 7},
        {8, 6, 7, 5},
        {3, 3, 3, 3},
        {6, 6, 6, 6},
        {5, 8, 5, 3}
    };
    Matrix<int> m(4,8,&p[0][0]);
    TEST_ASSERT(m.medoid_naive() == 5);
    std::span<int> vec = m.row(5);
    TEST_ASSERT(std::equal(vec.begin(),vec.end(),std::array{3, 3, 3, 3}.begin()));

    float p_f[8][4] = {
        {1.0f, 2.0f, 3.0f, 4.0f},
        {4.0f, 5.0f, 6.0f, 7.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {2.0f, 3.0f, 5.0f, 7.0f},
        {8.0f, 6.0f, 7.0f, 5.0f},
        {3.0f, 3.0f, 3.0f, 3.0f},
        {6.0f, 6.0f, 6.0f, 6.0f},
        {5.0f, 8.0f, 5.0f, 3.0f}
    };
    Matrix<float> n(4,8,&p_f[0][0]);
    TEST_ASSERT(n.medoid_naive() == 5);
    std::span<float> vec_f = n.row(5);
    TEST_ASSERT(std::equal(vec_f.begin(),vec_f.end(),std::array{3.0f, 3.0f, 3.0f, 3.0f}.begin()));



}

void test_find_medoid(){
    float p_f[8][4] = {
        {1.0f, 2.0f, 3.0f, 4.0f},
        {4.0f, 5.0f, 6.0f, 7.0f},
        {1.0f, 1.0f, 1.0f, 1.0f},
        {2.0f, 3.0f, 5.0f, 7.0f},
        {8.0f, 6.0f, 7.0f, 5.0f},
        {3.0f, 3.0f, 3.0f, 3.0f},
        {6.0f, 6.0f, 6.0f, 6.0f},
        {5.0f, 8.0f, 5.0f, 3.0f}
    };

    std::vector<float> vecs(&p_f[0][0], &p_f[0][0] + 8*4);
    std::vector<float> vecs_filters ={1.0f,2.0f,3.0f,1.0f,2.0f,3.0f,1.0f,2.0f};
    std::unordered_set<float> filters = {1.0f,2.0f,3.0f};
    Matrix<float> f_m(4,8,&vecs, &vecs_filters, &filters);
    size_t t=5;

    std::unordered_map<float, int> Medoid;          
    std::unordered_map<float, std::vector<int>> Pf;
    f_m.find_medoid(t, Medoid, Pf);
    for(auto &f: filters){
        auto it = Medoid.find(f);
        TEST_ASSERT(it != Medoid.end()); //test that filter has been successfully been inserted
    }



}
