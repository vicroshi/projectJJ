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
    std::string file_path1="../datasets/test/test.fvecs";
    float *result1=read_from_file<float>(file_path1,&dim1,&vecs1);
    TEST_ASSERT(result1 != nullptr);
    TEST_ASSERT((std::is_same<decltype(result1), float*>::value)); //check if its a int*
    TEST_ASSERT(dim1==128);
    printf("vecs1: %ld\n",vecs1);
    TEST_ASSERT(vecs1==10);
    size_t dim2,vecs2;
    std::string file_path2="../datasets/test/test_groundtruth.ivecs";
    int *result2=read_from_file<int>(file_path2,&dim2,&vecs2);
    TEST_ASSERT(result2 != nullptr);
    TEST_ASSERT((std::is_same<decltype(result2), int*>::value)); //check if its a float*
    TEST_ASSERT(dim2==100);
    TEST_ASSERT(vecs2==10);
    size_t dim3,vecs3;
    std::string file_path3="./non_existing_file";
    float *result3=read_from_file<float>(file_path3,&dim3,&vecs3);
    TEST_ASSERT(result3 == nullptr); //check if its a nullptr*
    
    int *result4=read_from_file<int>(file_path3,&dim3,&vecs3);
    TEST_ASSERT(result4 == nullptr); //check if its a nullptr*

}

void test_recall(){
    std::vector<int> X={0,1,2,3},G={0,1};

    int k=2;
    double result=recall_k(k,X,G,0);
    // std::cout<<result<<std::endl;
    TEST_ASSERT(result==1.0);

    G.push_back(4);
    k=3;
    result=recall_k(k,X,G,0);
    // std::cout<<result<<std::endl;
    TEST_ASSERT(result==(double)2/3);

    X.push_back(4);
    k=3;
    result=recall_k(k,X,G,0);
    // std::cout<<result<<std::endl;
    TEST_ASSERT(result==1.0);
}


void test_extract_data(){
    std::vector<std::vector<float>> data={
        {1.0f,1.33f,1.0f,2.0f,5.5f,8.0f},
        {1.0f,1.33f,2.0f,4.0f,2.1f,1.0f},
        {2.0f,1.33f,4.4f,2.1f,2.7f,7.0f},
        {2.0f,1.33f,5.1f,5.0f,1.1f,0.0f},
        {3.0f,1.33f,0.0f,1.0f,2.1f,3.0f},
        {3.0f,1.33f,0.0f,1.0f,2.1f,3.0f}
    };
    std::vector<float> expected_flattened_data = {
        1.0f, 2.0f, 5.5f, 8.0f,
        2.0f, 4.0f, 2.1f, 1.0f,
        4.4f, 2.1f, 2.7f, 7.0f,
        5.1f, 5.0f, 1.1f, 0.0f,
        0.0f, 1.0f, 2.1f, 3.0f,
        0.0f, 1.0f, 2.1f, 3.0f
    };
    std::vector<float>point_filter;
    std::vector<float>filters_set;
    std::vector<float>flattened_data;
    std::unordered_map<float, std::vector<int>> Pf;

    extract_base_vector_info(data,point_filter,filters_set,flattened_data,Pf);

    //test flattened data is properly formatted
    TEST_ASSERT(flattened_data==expected_flattened_data);

    //test that point's filter were extracted correctly
    std::vector<float> expected_point_filter={1.0f,1.0f,2.0f,2.0f,3.0f,3.0f};

    TEST_ASSERT(point_filter==expected_point_filter);

    //test to see if all filters were extracted
    std::vector<float> expected_filters_set = {1.0f, 2.0f, 3.0f};
    TEST_ASSERT(filters_set == expected_filters_set);

    //test that Pf has the right amount of points
    TEST_ASSERT(Pf[1.0f].size()==2); 
    TEST_ASSERT(Pf[2.0f].size()==2); 
    TEST_ASSERT(Pf[3.0f].size()==2); 

}


void test_extract_query(){
    std::vector<std::vector<float>> data={
        {1.0f,1.0f,1.33f,0.0f  ,1.0f,2.0f,5.5f,8.0f},
        {0.0f,-1.0f,-1.0f,0.0f ,2.0f,4.0f,2.1f,1.0f},
        {2.0f,1.33f,0.0f,0.0f  ,4.4f,2.1f,2.7f,7.0f},
        {1.0f,12.0f,1.0f,11.0f ,5.1f,5.0f,1.1f,0.0f},
        {1.0f,2.0f,0.0f,0.0f   ,0.0f,1.0f,2.1f,3.0f},
        {0.0f,-1.0f,-1.0f,0.0f ,0.0f,1.0f,2.1f,3.0f}
    };
    std::vector<float> expected_flattened_data = {
        1.0f, 2.0f, 5.5f, 8.0f,
        2.0f, 4.0f, 2.1f, 1.0f,
        5.1f, 5.0f, 1.1f, 0.0f,
        0.0f, 1.0f, 2.1f, 3.0f,
        0.0f, 1.0f, 2.1f, 3.0f
    };
    std::vector<float>point_filter;
    std::vector<int>query_type;
    std::vector<float>flattened_data;
    size_t filter_num=0,unfil_num=0;
    extract_query_vector_info(data,point_filter,query_type,flattened_data,filter_num,unfil_num);

    std::vector<float> expected_point_filter={1.0f,-1.0f,12.0f,2.0f,-1.0f};
    TEST_ASSERT(point_filter==expected_point_filter);

    std::vector<int> expected_query_type={1,0,1,1,0};
    TEST_ASSERT(query_type==expected_query_type);

    TEST_ASSERT(filter_num==3);
    TEST_ASSERT(unfil_num==2);

    TEST_ASSERT(flattened_data==expected_flattened_data);

}

void test_remove_negative_elements(){
    std::vector<std::vector<float>> data={
        {-1.0f,1.0f,2.0f,5.0,8.0f},
        {0.0f,-1.0f,-1.0f,0.0f ,2.0f,4.0f,2.0f,1.0f},
        {1.0f,-2.0f,0.0f,0.0f ,0.0f,1.0f,2.0f,3.0f},
        {-1.0f,-1.0f,-1.0f,0.0f ,0.0f,1.0f,2.0f,3.0f}
    };
    std::vector<std::vector<int>> result;
    remove_negative_elements(data,result);
    std::vector<std::vector<int>> expected_result={
        {1, 2, 5, 8},
        {0, 0, 2, 4, 2, 1},
        {1, 0, 0, 0, 1, 2, 3},
        {0, 0, 1, 2, 3}
    };

    TEST_ASSERT(result==expected_result);
}



void test_file_path(){

    size_t k=2,L=3,R=4;
    float a=1.4;
    std::string type="filtered_graph_";
    std::filesystem::path p1=get_file_path(k,L,R,a,type);
    auto it = p1.end();
    
    --it;
    std::filesystem::path last_component = *it;

    --it;
    std::filesystem::path second_last_component = *it;
    std::filesystem::path last_two_components = second_last_component / last_component;
    std::filesystem::path expected_path1= "graphs/filtered_graph_k_2_L_3_R_4_a_1_4_variation.bin";
    // std::cout<<last_two_components<<std::endl;
    // std::cout<<expected_path1<<std::endl;
    TEST_ASSERT(last_two_components == expected_path1) ;
    
    k=5,L=2,R=9;
    a=2.0;
    type="stitched_graph_";
    std::filesystem::path p2=get_file_path(k,L,R,a,type);
     it = p2.end();
    
    --it;
    last_component = *it;

    --it;
     second_last_component = *it;
     last_two_components = second_last_component / last_component;
    //  std::cout << last_two_components << std::endl;
        

    std::filesystem::path expected_path2= "graphs/stitched_graph_k_5_L_small_2_R_small_9_a_2_variation.bin";
//  std::cout<<expected_path2<<std::endl;
    TEST_ASSERT(last_two_components == expected_path2) ;
    


}