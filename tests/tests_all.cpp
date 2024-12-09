//
// Created by vic on 26/10/2024.
//
#include <acutest.h>
//edw vazoume ta prototypes olwn twn tests kai ta prosthetoume stin lista

/*   otan ftiaxnoume ena neo test file
 *   prepei na kanoume #define TEST_NO_MAIN
 *   to mono arxeio pou den to kanei #define einai auto edw
 *   meta vazoume ta function declaritions ws extern edw pera
 *   kai to prosthetoume stin lista
*/



//vamana tests
extern void test_greedy();
extern void test_k_closest();
extern void test_prune();
extern void test_vamana_index();
extern void test_init();
extern void test_save_graph();
extern void test_load_graph();
/*
 *
 *
 *
 */
//filtered tests
extern void test_filtered_greedy();
extern void test_filtered_prune();
extern void test_filtered_vamana_indexing();
extern void test_stitched();
/*
 *
 *
 *
 */

//utils tests
extern void test_fileType();
extern void test_read();
extern void test_recall();
extern void test_extract_data();
extern void test_extract_query();
extern void test_remove_negative_elements();
extern void test_file_path();

/*
 *
 *
 */

//matrix tests
extern void test_constructor();
extern void test_medoid();
extern void test_euclid();
extern void test_find_medoid();


TEST_LIST = {
    {"matrix_constructor", test_constructor},
    {"euclid", test_euclid},
    {"medoid_naive", test_medoid},
    {"init",test_init},
    {"greedy_search",test_greedy},
    {"file_Type",test_fileType},
    {"file_read",test_read},
    {"k_closest",test_k_closest},
    {"prune",test_prune},
    {"vamana_index",test_vamana_index},
    {"save_graph",test_save_graph},
    {"load_graph",test_load_graph},
    {"recall",test_recall},
    {"filtered_greedy",test_filtered_greedy},
    {"filtered_prune",test_filtered_prune},
    {"stitched",    test_stitched},
    {"find_medoid",test_find_medoid},
    {"extract_data",test_extract_data},
    {"extract_query",test_extract_query},
    {"remove_negative_elements",test_remove_negative_elements},
    {"filtered_vamana_indexing",test_filtered_vamana_indexing},
    {"file_path",test_file_path},
    {nullptr, nullptr}
};