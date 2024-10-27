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
    double d1,d2,d3;
    float p[4][4] = {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {1, 3, 10, 2},
        {2, 3, 5, 7}
    };
    Matrix<float> m(4,4,&p[0][0]);
//    std::cout << m.sq_euclid(0, 1) << std::endl;
    TEST_ASSERT(m.sq_euclid(m.get_row(0), m.get_row(1)) == 4);
    TEST_ASSERT(m.sq_euclid(m.get_row(0), m.get_row(2)) == 114);
    TEST_ASSERT(m.sq_euclid(m.get_row(0), m.get_row(3)) == 87);
    TEST_ASSERT(m.sq_euclid(m.get_row(2), m.get_row(3)) == 51);
    int i[4][4] = {
        {0, 0, 0, 0},
        {1, 1, 1, 1},
        {1, 3, 10, 2},
        {2, 3, 5, 7}
    };
    Matrix<int> n(4,4,&i[0][0]);
    // Matrix<int> n(4,4,reinterpret_cast<int*>(&p[0][0]));
    
//    std::cout << n.sq_euclid(m.get_row(0), m.get_row(1)) << std::endl;
    TEST_ASSERT(n.sq_euclid(n.get_row(0), n.get_row(1)) == 4);
    TEST_ASSERT(n.sq_euclid(n.get_row(0), n.get_row(2)) == 114);
    TEST_ASSERT(n.sq_euclid(n.get_row(0), n.get_row(3)) == 87);
    TEST_ASSERT(n.sq_euclid(n.get_row(2), n.get_row(3)) == 51);
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
    std::span<int> vec = m.get_row(5);
    TEST_ASSERT(std::equal(vec.begin(),vec.end(),std::array{3, 3, 3, 3}.begin()));
    TEST_CASE("distance matrix");
    double cdist[8][8];
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            cdist[i][j] = Matrix<int>::sq_euclid(m.get_row(i), m.get_row(j));
        }
    }
    double true_dist[8][8] = {
        {0.00, 5.10, 4.24, 4.47, 8.31, 3.46, 7.35, 6.48},
        {5.10, 0.00, 8.19, 3.32, 3.61, 7.35, 5.48, 4.36},
        {4.24, 8.19, 0.00, 7.35, 10.95, 2.83, 7.35, 8.31},
        {4.47, 3.32, 7.35, 0.00, 6.48, 5.83, 7.35, 5.39},
        {8.31, 3.61, 10.95, 6.48, 0.00, 9.85, 6.16, 3.61},
        {3.46, 7.35, 2.83, 5.83, 9.85, 0.00, 5.20, 7.35},
        {7.35, 5.48, 7.35, 7.35, 6.16, 5.20, 0.00, 6.93},
        {6.48, 4.36, 8.31, 5.39, 3.61, 7.35, 6.93, 0.00}
    };
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            TEST_CHECK(cdist[i][j] == true_dist[i][j]);
        }
    }
}
