//
// Created by vic on 25/10/2024.
//
#define TEST_NO_MAIN
#include <distance.h>
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
    float p[4][8] = {
         {0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f},
         {1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {1.0f, 3.0f, 10.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f},
        {2.0f, 3.0f, 5.0f, 7.0f, 0.0f, 0.0f, 0.0f, 0.0f}
    };
    Matrix<float> m(4,4,&p[0][0]);
//    std::cout<<'\n';

    std::cout << sq_euclid(&p[0][0], &p[1][0], 8) << std::endl;
    TEST_CHECK(sq_euclid(&p[0][0], &p[1][0], 8) == 4);
    TEST_CHECK(sq_euclid(&p[0][0], &p[2][0], 8) == 114);
    TEST_CHECK(sq_euclid(&p[0][0], &p[3][0], 8) == 87);
    TEST_CHECK(sq_euclid(&p[2][0], &p[3][0], 8) == 51);
//    std::cout<<Matrix<float>::sq_euclid(m.row(0), m.row(1))<<std::endl;
//    TEST_CHECK(Matrix<float>::sq_euclid(m.row(0), m.row(1)) == 4);
//    TEST_CHECK(Matrix<float>::sq_euclid(m.row(0), m.row(2)) == 114);
//    TEST_CHECK(Matrix<float>::sq_euclid(m.row(0), m.row(3)) == 87);
//    TEST_CHECK(Matrix<float>::sq_euclid(m.row(2), m.row(3)) == 51);
//    int i[4][4] = {
//        {0, 0, 0, 0},
//        {1, 1, 1, 1},
//        {1, 3, 10, 2},
//        {2, 3, 5, 7}
//    };
//    Matrix<int> n(4,4,&i[0][0]);
//    // Matrix<int> n(4,4,reinterpret_cast<int*>(&p[0][0]));
//
////    std::cout << n.sq_euclid(m.row(0), m.row(1)) << std::endl;
//    TEST_ASSERT(n.sq_euclid(n.row(0), n.row(1)) == 4);
//    TEST_ASSERT(n.sq_euclid(n.row(0), n.row(2)) == 114);
//    TEST_ASSERT(n.sq_euclid(n.row(0), n.row(3)) == 87);
//    TEST_ASSERT(n.sq_euclid(n.row(2), n.row(3)) == 51);
}


void test_medoid() {
//    int p[8][4] = {
//        {1, 2, 3, 4},
//        {4, 5, 6, 7},
//        {1, 1, 1, 1},
//        {2, 3, 5, 7},
//        {8, 6, 7, 5},
//        {3, 3, 3, 3},
//        {6, 6, 6, 6},
//        {5, 8, 5, 3}
//    };
//    Matrix<int> m(4,8,&p[0][0]);
//    TEST_ASSERT(m.medoid_naive() == 5);
//    std::span<int> vec = m.row(5);
//    TEST_ASSERT(std::equal(vec.begin(),vec.end(),std::array{3, 3, 3, 3}.begin()));
//    TEST_CASE("distance matrix");
//    double cdist[8][8];
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            cdist[i][j] = Matrix<int>::sq_euclid(m.row(i), m.row(j));
//        }
//    }
//    double true_dist[8][8] = {
//        {  0.0,  36.0,  14.0,  15.0,  82.0,   6.0,  54.0,  57.0 },
//        { 36.0,   0.0,  86.0,   9.0,  22.0,  30.0,   6.0,  27.0 },
//        { 14.0,  86.0,   0.0,  57.0, 126.0,  16.0, 100.0,  85.0 },
//        { 15.0,   9.0,  57.0,   0.0,  53.0,  21.0,  27.0,  50.0 },
//        { 82.0,  22.0, 126.0,  53.0,   0.0,  54.0,   6.0,  21.0 },
//        {  6.0,  30.0,  16.0,  21.0,  54.0,   0.0,  36.0,  33.0 },
//        { 54.0,   6.0, 100.0,  27.0,   6.0,  36.0,   0.0,  15.0 },
//        { 57.0,  27.0,  85.0,  50.0,  21.0,  33.0,  15.0,   0.0 }
//    };
//    std::cout<<'\n';
//    bool flag = true;
//    for (int i = 0; i < 8; i++) {
//        for (int j = 0; j < 8; j++) {
//            if(cdist[i][j] != true_dist[i][j]) {
//                flag = false;
//            }
////            std::cout << cdist[i][j] << " ";
//        }
////        std::cout << std::endl;
//    }
//    TEST_CHECK(flag==true);
}
