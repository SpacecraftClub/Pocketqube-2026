#include "unity.h"
#include "stats.h"
#include <stdio.h>

static int32_t int_data_sorted[] = {1,2,3,4,5,6,7,8,9};
static int32_t int_data_mixed[] = {9,5,4,7,6,3,2,1,8};
static int32_t int_data_even_sorted[] = {1,2,3,4,5,6,7,8,9,10};
static int32_t int_data_even_mixed[] = {3,9,4,5,1,8,2,10,6,7};

static float float_data_sorted[] = {1,2,3,4,5,6,7,8,9};
static float float_data_mixed[] = {9,5,4,7,6,3,2,1,8};
static float float_data_even_sorted[] = {1,2,3,4,5,6,7,8,9,10};
static float float_data_even_mixed[] = {3,9,4,5,1,8,2,10,6,7};

static int32_t int_buff_9[9];
static int32_t int_buff_10[10];

static float float_buff_9[9];
static float float_buff_10[10];

void setUp(void) {
  // set stuff up here
}

void tearDown(void) {
  // clean stuff up here
}

void test_function_int_median_odd_no_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindMedian(int_data_sorted, 9, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, median);
}

void test_function_float_median_odd_no_sort(void) {
    float median = 0;
    StatsOperation retVal = FindMedian(float_data_sorted, 9, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.0, median);
}

void test_function_int_median_even_no_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindMedian(int_data_even_sorted, 10, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, median);
}

void test_function_float_median_even_no_sort(void) {
    float median = 0;
    StatsOperation retVal = FindMedian(float_data_even_sorted, 10, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, median);
}

void test_function_int_q1_odd_no_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_sorted, 9, 1, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(2, quartile);
}

void test_function_float_q1_odd_no_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_sorted, 9, 1, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(2.5, quartile);
}

void test_function_int_q1_even_no_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_even_sorted, 10, 1, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(3, quartile);
}

void test_function_float_q1_even_no_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_even_sorted, 10, 1, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(3.0, quartile);
}

void test_function_int_q2_odd_no_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindQuartile(int_data_sorted, 9, 2, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, median);
}

void test_function_float_q2_odd_no_sort(void) {
    float median = 0;
    StatsOperation retVal = FindQuartile(float_data_sorted, 9, 2, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, median);
}

void test_function_int_q2_even_no_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindQuartile(int_data_even_sorted, 10, 2, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, median);
}

void test_function_float_q2_even_no_sort(void) {
    float median = 0;
    StatsOperation retVal = FindQuartile(float_data_even_sorted, 10, 2, &median, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, median);
}

void test_function_int_q3_odd_no_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_sorted, 9, 3, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(7, quartile);
}

void test_function_float_q3_odd_no_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_sorted, 9, 3, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(7.5, quartile);
}

void test_function_int_q3_even_no_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_even_sorted, 10, 3, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(8, quartile);
}

void test_function_float_q3_even_no_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_even_sorted, 10, 3, &quartile, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(8.0, quartile);
}

void test_function_float_mean_odd_STD_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_sorted, 9, &mean, false, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, mean);
}

void test_function_float_mean_even_STD_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_even_sorted, 10, &mean, false, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, mean);
}

void test_function_float_mean_odd_high_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_sorted, 9, &mean, true, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, mean);
}

void test_function_float_mean_even_high_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_even_sorted, 10, &mean, true, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, mean);
}

int runUnityTests(void) {
  UNITY_BEGIN();
  RUN_TEST(test_function_int_median_odd_no_sort);
  RUN_TEST(test_function_float_median_odd_no_sort);
  RUN_TEST(test_function_int_median_even_no_sort);
  RUN_TEST(test_function_float_median_even_no_sort);
  RUN_TEST(test_function_int_q1_odd_no_sort);
  RUN_TEST(test_function_float_q1_odd_no_sort);
  RUN_TEST(test_function_int_q1_even_no_sort);
  RUN_TEST(test_function_float_q1_even_no_sort);
  RUN_TEST(test_function_int_q2_odd_no_sort);
  RUN_TEST(test_function_float_q2_odd_no_sort);
  RUN_TEST(test_function_int_q2_even_no_sort);
  RUN_TEST(test_function_float_q2_even_no_sort);
  RUN_TEST(test_function_int_q3_odd_no_sort);
  RUN_TEST(test_function_float_q3_odd_no_sort);
  RUN_TEST(test_function_int_q3_even_no_sort);
  RUN_TEST(test_function_float_q3_even_no_sort);
  RUN_TEST(test_function_float_mean_odd_STD_Precision_no_sort);
  RUN_TEST(test_function_float_mean_even_STD_Precision_no_sort);
  RUN_TEST(test_function_float_mean_odd_high_Precision_no_sort);
  RUN_TEST(test_function_float_mean_even_high_Precision_no_sort);
  return UNITY_END();
}

void setup() {
  runUnityTests();
}
void loop() {}


void WinMain(){
    setup();
}

#ifdef RUNNING_WIN_NATIVE
#pragma message "Running tests on Native Windows!"
#else
#pragma message "Running tests on hardware"
#endif