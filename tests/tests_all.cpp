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
/*
 *
 *
 *
 */
//filtered tests
extern void test_filtered_greedy();
/*
 *
 *
 *
 */

//utils tests
extern void test_fileType();
extern void test_read();
extern void test_recall();
/*
 *
 *
 */

//matrix tests
extern void test_constructor();
extern void test_medoid();
extern void test_euclid();



TEST_LIST = {
    {"matrix_constructor", test_constructor},
    {"euclid", test_euclid},
    {"medoid_naive", test_medoid},
    {"greedy_search",test_greedy},
    {"file_Type",test_fileType},
    {"file_read",test_read},
    {"k_closest",test_k_closest},
    {"prune",test_prune},
    {"vamana_index",test_vamana_index},
    {"recall",test_recall},
    {"filtered_greedy",test_filtered_greedy},
    {nullptr, nullptr}
};