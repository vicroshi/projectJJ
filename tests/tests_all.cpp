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
/*
 *
 *
 *
 */

//utils tests
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
    {nullptr, nullptr}
};