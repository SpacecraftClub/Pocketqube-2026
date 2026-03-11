#define RUNNING_UNIT_TESTS true

#include "unity.h"
#include "stats.h"
#include <stdio.h>

static int32_t int_data_sorted[] = {1,2,3,4,5,6,7,8,9};
static int32_t int_data_mixed[] =  {9,5,4,7,3,6,2,1,8};
static int32_t int_data_even_sorted[] = {1,2,3,4,5,6,7,8,9,10};
static int32_t int_data_even_mixed[] =  {3,9,4,5,1,8,2,10,6,7};

static float float_data_sorted[] = {1,2,3,4,5,6,7,8,9};
static float float_data_mixed[] = {9,5,4,7,6,3,2,1,8};
static float float_data_even_sorted[] = {1,2,3,4,5,6,7,8,9,10};
static float float_data_even_mixed[] = {3,9,4,5,1,8,2,10,6,7};

static int32_t int_buff_9[9];
static int32_t int_buff_10[10];

static float float_buff_9[9];
static float float_buff_10[10];


/*
Sensor Data that causes Crash 3-10-26
        accel_x: 0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,
        accel_y: -0.04,-0.04,-0.04,-0.04,-0.04,-0.03,-0.04,-0.04,-0.04,
        accel_z: 1.03,1.03,1.04,1.03,1.04,1.03,1.03,1.04,1.03,
        gyro_x: -0.43,-0.43,-0.43,-0.43,-0.43,-0.43,-0.37,-0.43,-0.43,
        gyro_y: 0.18,0.12,0.12,0.12,0.12,0.18,0.18,0.18,0.12,
        gyro_z: -0.55,-0.55,-0.55,-0.55,-0.55,-0.55,-0.55,-0.55,-0.55,
        temp_LMS: 22.00,22.00,22.00,22.00,22.00,22.00,22.00,22.00,22.00,
*/

static float rawData[7][9] = {
    {0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02,0.02},
    {-0.04,-0.04,-0.04,-0.04,-0.04,-0.03,-0.04,-0.04,-0.04},
    {1.03,1.03,1.04,1.03,1.04,1.03,1.03,1.04,1.03},
    {-0.43,-0.43,-0.43,-0.43,-0.43,-0.43,-0.37,-0.43,-0.43},
    {0.18,0.12,0.12,0.12,0.12,0.18,0.18,0.18,0.12},
    {-0.55,-0.55,-0.55,-0.55,-0.55,-0.55,-0.55,-0.55,-0.55},
    {22.00,22.00,22.00,22.00,22.00,22.00,22.00,22.00,22.00}
};

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
    TEST_ASSERT_EQUAL_FLOAT(5.0, median);
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
    TEST_ASSERT_EQUAL_FLOAT(5.5, median);
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
    TEST_ASSERT_EQUAL_FLOAT(2.5, quartile);
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
    TEST_ASSERT_EQUAL_FLOAT(3.0, quartile);
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
    TEST_ASSERT_EQUAL_FLOAT(5, median);
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
    TEST_ASSERT_EQUAL_FLOAT(5.5, median);
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
    TEST_ASSERT_EQUAL_FLOAT(7.5, quartile);
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
    TEST_ASSERT_EQUAL_FLOAT(8.0, quartile);
}

void test_function_float_mean_odd_STD_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_sorted, 9, &mean, false, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5, mean);
}

void test_function_float_mean_even_STD_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_even_sorted, 10, &mean, false, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5.5, mean);
}

void test_function_float_mean_odd_high_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_sorted, 9, &mean, true, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5, mean);
}

void test_function_float_mean_even_high_Precision_no_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_even_sorted, 10, &mean, true, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5.5, mean);
}

void test_function_int_mean_odd_no_sort(void){
    int32_t mean = 0;
    StatsOperation retVal = CalculateMean(int_data_sorted, 9, &mean, false, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, mean);
}

void test_function_int_mean_even_no_sort(void){
    int32_t mean = 0;
    StatsOperation retVal = CalculateMean(int_data_even_sorted, 10, &mean, false,  false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, mean);
}



void test_function_int_median_odd_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindMedian(int_data_mixed, 9, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, median);
}

void test_function_float_median_odd_sort(void) {
    float median = 0;
    StatsOperation retVal = FindMedian(float_data_mixed, 9, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5.0, median);
}

void test_function_int_median_even_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindMedian(int_data_even_mixed, 10, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, median);
}

void test_function_float_median_even_sort(void) {
    float median = 0;
    StatsOperation retVal = FindMedian(float_data_even_mixed, 10, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5.5, median);
}

void test_function_int_q1_odd_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_mixed, 9, 1, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(2, quartile);
}

void test_function_float_q1_odd_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_mixed, 9, 1, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(2.5, quartile);
}

void test_function_int_q1_even_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_even_mixed, 10, 1, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(3, quartile);
}

void test_function_float_q1_even_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_even_mixed, 10, 1, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(3.0, quartile);
}

void test_function_int_q2_odd_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindQuartile(int_data_mixed, 9, 2, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, median);
}

void test_function_float_q2_odd_sort(void) {
    float median = 0;
    StatsOperation retVal = FindQuartile(float_data_mixed, 9, 2, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5, median);
}

void test_function_int_q2_even_sort(void) {
    int32_t median = 0;
    StatsOperation retVal = FindQuartile(int_data_even_mixed, 10, 2, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, median);
}

void test_function_float_q2_even_sort(void) {
    float median = 0;
    StatsOperation retVal = FindQuartile(float_data_even_mixed, 10, 2, &median);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5.5, median);
}

void test_function_int_q3_odd_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_mixed, 9, 3, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(7, quartile);
}

void test_function_float_q3_odd_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_mixed, 9, 3, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(7.5, quartile);
}

void test_function_int_q3_even_sort(void) {
    int32_t quartile = 0;
    StatsOperation retVal = FindQuartile(int_data_even_mixed, 10, 3, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(8, quartile);
}

void test_function_float_q3_even_sort(void) {
    float quartile = 0;
    StatsOperation retVal = FindQuartile(float_data_even_mixed, 10, 3, &quartile);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(8.0, quartile);
}

void test_function_float_mean_odd_STD_Precision_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_mixed, 9, &mean, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5, mean);
}

void test_function_float_mean_even_STD_Precision_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_even_mixed, 10, &mean, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5.5, mean);
}

void test_function_float_mean_odd_high_Precision_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_mixed, 9, &mean, true);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5, mean);
}

void test_function_float_mean_even_high_Precision_sort(void){
    float mean = 0;
    StatsOperation retVal = CalculateMean(float_data_even_mixed, 10, &mean, true);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT(5.5, mean);
}

void test_function_int_mean_odd_sort(void){
    int32_t mean = 0;
    StatsOperation retVal = CalculateMean(int_data_mixed, 9, &mean, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5, mean);
}

void test_function_int_mean_even_sort(void){
    int32_t mean = 0;
    StatsOperation retVal = CalculateMean(int_data_even_mixed, 10, &mean, false);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL(5.5, mean);
}



void test_sort_float_sorted_list(void){
    float outArry1[9];
    float outArry2[10];

    StatsOperation retVal = SortData(float_data_sorted, outArry1, 9);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    retVal = SortData(float_data_even_sorted, outArry2, 10);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(float_data_sorted, outArry1, 9);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(float_data_even_sorted, outArry2, 10);
}

void test_sort_float_unsorted_list(void){
    float outArry1[9];
    float outArry2[10];

    StatsOperation retVal = SortData(float_data_mixed, outArry1, 9);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    retVal = SortData(float_data_even_mixed, outArry2, 10);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(float_data_sorted, outArry1, 9);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(float_data_even_sorted, outArry2, 10);
}

void test_sort_int_sorted_list(void){
    int32_t outArry1[9];
    int32_t outArry2[10];

    StatsOperation retVal = SortData(int_data_sorted, outArry1, 9);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    retVal = SortData(int_data_even_sorted, outArry2, 10);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(int_data_sorted, outArry1, 9);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(int_data_even_sorted, outArry2, 10);
}

void test_sort_int_unsorted_list(void){
    int32_t outArry1[9];
    int32_t outArry2[10];

    StatsOperation retVal = SortData(int_data_mixed, outArry1, 9);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    retVal = SortData(int_data_even_mixed, outArry2, 10);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");

    TEST_ASSERT_EQUAL_FLOAT_ARRAY(int_data_sorted, outArry1, 9);
    TEST_ASSERT_EQUAL_FLOAT_ARRAY(int_data_even_sorted, outArry2, 10);
}


void test_function_float_mean_with_outliers(void){
    const uint16_t testValLen = 8;
    float mean = 0, q1 = 0, q3 = 0;
    StatsOperation retVal;
    float input[testValLen] = {1,1000,1004,1008,1002,1006,1010.5, 100000};
    float output[testValLen];
    float sorted[testValLen] = {1,1000, 1002,1004,1006,1008,1010.5, 100000};

    retVal = SortData(input, output, testValLen);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function sort returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT_ARRAY_MESSAGE(sorted, output, testValLen, "List Sorted Wrong");

    retVal = FindQuartile(input, testValLen, 1, &q1);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function Q1 returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1001, q1, "Incorrect Q1");

    retVal = FindQuartile(input, testValLen, 3, &q1);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function Q3 returned Non OK message");
    TEST_ASSERT_EQUAL_FLOAT_MESSAGE(1009.25, q1, "Incorrect Q3");

    retVal = CalculateMean(input, testValLen, &mean);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function mean returned Non OK message");


    TEST_ASSERT_EQUAL_FLOAT((1000.0+1004.0+1008.0+1002.0+1006.0+1010.5)/6.0, mean);
}

void test_function_int_mean_with_outliers(void){
    const uint16_t testValLen = 8;
    int32_t mean = 0, q1 = 0, q3 = 0;
    StatsOperation retVal;
    int32_t input[testValLen] = {1,1000,1004,1008,1002,1006,1010, 100000};
    int32_t output[testValLen];
    int32_t sorted[testValLen] = {1,1000, 1002,1004,1006,1008,1010, 100000};

    retVal = SortData(input, output, testValLen);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function sort returned Non OK message");
    TEST_ASSERT_EQUAL_INT32_ARRAY_MESSAGE(sorted, output, testValLen, "List Sorted Wrong");

    retVal = FindQuartile(input, testValLen, 1, &q1);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function Q1 returned Non OK message");
    TEST_ASSERT_EQUAL_MESSAGE(1001, q1, "Incorrect Q1");

    retVal = FindQuartile(input, testValLen, 3, &q1);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function Q3 returned Non OK message");
    TEST_ASSERT_EQUAL_MESSAGE(1009, q1, "Incorrect Q3");

    retVal = CalculateMean(input, testValLen, &mean);
    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function mean returned Non OK message");


    TEST_ASSERT_EQUAL_FLOAT(1005, mean);
}


void test_sort_int32_overflow(void){
    const uint16_t testValLen = 6;
    int32_t median;
    int32_t testVals[testValLen] = {(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFF};
    int32_t output[testValLen];
    int32_t sorted[testValLen] =  {(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFF};

    StatsOperation retVal = SortData(testVals, output, testValLen);

    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL_INT32_ARRAY(sorted, output, testValLen);
}

void test_median_int32_overflow(void){
    const uint16_t testValLen = 6;
    int32_t median;
    int32_t testVals[testValLen] = {(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFF};

    StatsOperation retVal = FindMedian(testVals, testValLen, &median, true);

    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL((int32_t) 0x7FFFFFFE, median);
}

void test_q1_int32_overflow(void){
    const uint16_t testValLen = 6;
    int32_t median;
    int32_t testVals[testValLen] = {(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFF};

    StatsOperation retVal = FindQuartile(testVals, testValLen, 1, &median, true);

    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL((int32_t) 0x7FFFFFFD, median);
}

void test_q3_int32_overflow(void){
    const uint16_t testValLen = 6;
    int32_t median;
    int32_t testVals[testValLen] = {(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFF};

    StatsOperation retVal = FindQuartile(testVals, testValLen, 3, &median, true);

    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL((int32_t) 0x7FFFFFFF, median);
}

void test_mean_int32_overflow(void){
    const uint16_t testValLen = 6;
    int32_t mean;
    int32_t testVals[testValLen] = {(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFD,(int32_t) 0x7FFFFFFF,(int32_t) 0x7FFFFFFF};
    
    StatsOperation retVal = CalculateMean(testVals, testValLen, &mean, false,  true);

    TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, "Function returned Non OK message");
    TEST_ASSERT_EQUAL((int32_t) 0x7FFFFFFE, mean);
}

void test_raw_data(void){
    float mean;
    char outMsg[64];
    for(uint8_t dataType = 0; dataType < 7; dataType++){
        StatsOperation retVal = CalculateMean((float *) rawData[dataType], 9, &mean, false,  true);
        snprintf(outMsg, 64, "Data set %d returned non-OK message", dataType);
        TEST_ASSERT_EQUAL_MESSAGE(STATS_OPERATION_OK, retVal, outMsg);
    }
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
  RUN_TEST(test_function_int_mean_odd_no_sort);
  RUN_TEST(test_function_int_mean_even_no_sort);
  RUN_TEST(test_function_int_median_odd_sort);
  RUN_TEST(test_function_float_median_odd_sort);
  RUN_TEST(test_function_int_median_even_sort);
  RUN_TEST(test_function_float_median_even_sort);
  RUN_TEST(test_function_int_q1_odd_sort);
  RUN_TEST(test_function_float_q1_odd_sort);
  RUN_TEST(test_function_int_q1_even_sort);
  RUN_TEST(test_function_float_q1_even_sort);
  RUN_TEST(test_function_int_q2_odd_sort);
  RUN_TEST(test_function_float_q2_odd_sort);
  RUN_TEST(test_function_int_q2_even_sort);
  RUN_TEST(test_function_float_q2_even_sort);
  RUN_TEST(test_function_int_q3_odd_sort);
  RUN_TEST(test_function_float_q3_odd_sort);
  RUN_TEST(test_function_int_q3_even_sort);
  RUN_TEST(test_function_float_q3_even_sort);
  RUN_TEST(test_function_float_mean_odd_STD_Precision_sort);
  RUN_TEST(test_function_float_mean_even_STD_Precision_sort);
  RUN_TEST(test_function_float_mean_odd_high_Precision_sort);
  RUN_TEST(test_function_float_mean_even_high_Precision_sort);
  RUN_TEST(test_function_int_mean_odd_sort);
  RUN_TEST(test_function_int_mean_even_sort);
  RUN_TEST(test_sort_float_sorted_list);
  RUN_TEST(test_sort_float_unsorted_list);
  RUN_TEST(test_sort_int_sorted_list);
  RUN_TEST(test_sort_int_unsorted_list);

  RUN_TEST(test_function_float_mean_with_outliers);
  RUN_TEST(test_function_int_mean_with_outliers);

  RUN_TEST(test_sort_int32_overflow);
  RUN_TEST(test_q1_int32_overflow);
  RUN_TEST(test_q3_int32_overflow);
  RUN_TEST(test_median_int32_overflow);
  RUN_TEST(test_mean_int32_overflow);

  RUN_TEST(test_raw_data);
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