#ifndef STATS_H
#define STATS_H
#include <stdint.h>

typedef enum StatsOperation: uint8_t {
    STATS_OPERATION_OK = 0x00,
    STATS_OPERATION_ERROR_INSUFFICIENT_DATA = 0x01,
    STATS_OPERATION_ERROR_NULL_PTR = 0x02,
    STATS_OPERAITON_ERROR_INVALID_QUARTILE = 0x04,
    STATS_OPERAITON_ERROR_RESERVED_1 = 0x08,
    STATS_OPERAITON_ERROR_RESERVED_2 = 0x10,
    STATS_OPERAITON_ERROR_RESERVED_3 = 0x20,
    STATS_OPERATION_ERROR_UNKNOWN = 0x40,
    STATS_OPERATION_INCOMPLETE = 0x80
} StatsOperation;

/*
Sorts the data into a fresh array
@param data the list to be sorted
@param output the output list
@param len the length of the arrays
@returns if the operation succeeded or failed
*/
template<typename T>
StatsOperation SortData(T* data, T* output, uint16_t len);

/*
Calculates the median for the data at hand
@param data the array of data
@param len the length of the array
@param median the return value of the median
@param shouldSortFirst "true" if the data is unsorted
@returns if the operation succeeded or failed
*/
template<typename T>
StatsOperation FindMedian(T* data, uint16_t len, T* median, bool shouldSortFirst = true);

/*
Calculates the desired quartile for the data at hand
@param data the array of data
@param len the length of the array
@param quartileNumber the desired quartile (1st, 2nd, 3rd)
@param quartileValue the return value of the quartile
@param shouldSortFirst "true" if the data is unsorted
@returns if the operation succeeded or failed
*/
template<typename T>
StatsOperation FindQuartile(T* data, uint16_t len, uint16_t quartileNumber, T* quartileValue, bool shouldSortFirst = true);

/*
Calculates the Mean of the data (excluding outliers)
@param data the array of data
@param len the length of the array
@param mean the mean of the data
@param highPrecision if a double should be used for calculating the average (Higher precision at the cost of many clock cycles)
@param shouldSortFirst "true" if the data is unsorted
@returns if the operation succeeded or failed
*/
template<typename T>
StatsOperation CalculateMean(T* data, uint16_t len, T* mean, bool highPrecision = false, bool shouldSortFirst = true);

#endif