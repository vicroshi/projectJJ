//
// Created by vic on 25/10/2024.
//
#define TEST_NO_MAIN

#include <acutest.h>
#include <iostream>
#include "utils.h"
//#include <database.h>

void test_fileType(){
    std::string file1="file1.fvecs", file2="file2.ivecs",file3="file3";
    TEST_ASSERT(getFileExtension(file1)=="fvecs");
    TEST_ASSERT(getFileExtension(file2)=="ivecs");
    TEST_ASSERT(getFileExtension(file3)!="ivecs" && getFileExtension(file3)!="fvecs");
}

void test_read(){
    size_t dim1,vecs1;
    std::string file_path1="../datasets/siftsmall/siftsmall_base.fvecs";
    int *result1=read_from_file<int>(file_path1,&dim1,&vecs1);
    TEST_ASSERT(result1 != nullptr);
    TEST_ASSERT((std::is_same<decltype(result1), int*>::value)); //check if its a int*

    size_t dim2,vecs2;
    std::string file_path2="../datasets/siftsmall/siftsmall_groundtruth.ivecs";
    float *result2=read_from_file<float>(file_path2,&dim2,&vecs2);
    TEST_ASSERT(result2 != nullptr);
    TEST_ASSERT((std::is_same<decltype(result2), float*>::value)); //check if its a float*
    
    size_t dim3,vecs3;
    std::string file_path3="non_existing_file";
    float *result3=read_from_file<float>(file_path3,&dim3,&vecs3);
    TEST_ASSERT(result3 == nullptr); //check if its a nullptr*
    
    int *result4=read_from_file<int>(file_path3,&dim3,&vecs3);
    TEST_ASSERT(result4 == nullptr); //check if its a nullptr*

}

void test_recall(){
    std::vector<int> X={0,1,2,3},G={0,1};

    int k=2;
    double result=recall_k(k,X,G);
    // std::cout<<result<<std::endl;
    TEST_ASSERT(result==1.0);

    G.push_back(4);
    k=3;
    result=recall_k(k,X,G);
    // std::cout<<result<<std::endl;
    TEST_ASSERT(result==(double)2/3);

    X.push_back(4);
    k=3;
    result=recall_k(k,X,G);
    // std::cout<<result<<std::endl;
    TEST_ASSERT(result==1.0);
}