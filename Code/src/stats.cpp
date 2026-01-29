#include "stats.h"

/*
NF 1-29-2026
BYU Spacecraft Club
Stats Library for cleaning data

TODO:
FindMedian & Quartile fns need to be reworked so that they adhere to the standard made in findMean
sort data (quicksort or insertion sort)
Unit Tests
*/

#pragma message "Stats.cpp compiled"

#define IS_INT_ODD(num) ((num % 2) != 0)
#define STATS_OPERATION_CONCATINATE_RESULTS(val, fn) val = ((StatsOperation)((val) | (fn)))

void testInclude(){
    return;
}

StatsOperation SortData(float* data, float* output, uint16_t len){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }
    

    return retVal;
}

StatsOperation SortData(int32_t* data, int32_t* output, uint16_t len){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;

    return retVal;
}

StatsOperation FindMedian(float* data, uint16_t len, float* median, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    float* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new float[len];
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, SortData(data, sortedData, len));
        } else{
            sortedData = data;
        }
    }

    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(IS_INT_ODD(len)){
            *median = sortedData[len / 2]; // get the middle index
        } else{
            *median = sortedData[len / 2 - 1];
            *median += sortedData[len/2];
            *median /= 2;
        }
        retVal = STATS_OPERATION_OK;
    }

    if(sortedData != data && sortedData != nullptr){
        delete sortedData;
    }

    return retVal;
}

StatsOperation FindMedian(int32_t* data, uint16_t len, int32_t* median, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    int32_t* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new int32_t[len];
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, SortData(data, sortedData, len));
        } else{
            sortedData = data;
        }
    }

    if(retVal = STATS_OPERATION_INCOMPLETE){
        if(IS_INT_ODD(len)){
            *median = sortedData[len / 2]; // get the middle index
        } else{
            int64_t tempVal = sortedData[len / 2 - 1];
            tempVal += sortedData[len/2];
            *median = tempVal / 2;
        }
        retVal = STATS_OPERATION_OK;
    }

    if(sortedData != data && sortedData != nullptr){
        delete sortedData;
    }

    return retVal;
}

StatsOperation FindQuartile(float* data, uint16_t len, uint16_t quartileNumber, float* quartileValue, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    float* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new float[len];
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, SortData(data, sortedData, len));
        } else{
            sortedData = data;
        }
    }

    if(quartileNumber > 0 && quartileNumber < 4){
        if(quartileNumber == 2){
            retVal = FindMedian(sortedData, len, quartileValue, false);
        } else{
            uint16_t medianIndex = len / 2; // we always exclude the median so this works for even & odd
            if(quartileNumber == 1){
                FindMedian(sortedData, medianIndex, quartileValue, false); // this is equal to what we want
            } else{ // we know that quartileNumber = 3
                FindMedian(sortedData + medianIndex, len - medianIndex, quartileValue, false); // this is equal to what we want
            }
        }
        if(retVal == STATS_OPERATION_INCOMPLETE){
            retVal = STATS_OPERATION_OK;
        }
    } else{
        retVal = STATS_OPERAITON_ERROR_INVALID_QUARTILE;
    }

    if(sortedData != data && sortedData != nullptr){
        delete sortedData;
    }

    return retVal;
}

StatsOperation FindQuartile(int32_t* data, uint16_t len, uint16_t quartileNumber, int32_t* quartileValue, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    int32_t* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new int32_t[len];
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, SortData(data, sortedData, len));
        } else{
            sortedData = data;
        }
    }

    if(quartileNumber > 0 && quartileNumber < 4 && retVal == STATS_OPERATION_INCOMPLETE){
        if(quartileNumber == 2){
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindMedian(sortedData, len, quartileValue, false));
        } else{
            uint16_t medianIndex = len / 2; // we always exclude the median so this works for even & odd
            if(quartileNumber == 1){
                STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindMedian(sortedData, medianIndex, quartileValue, false)); // this is equal to what we want
            } else{ // we know that quartileNumber = 3
                STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindMedian(sortedData + medianIndex, len - medianIndex, quartileValue, false)); // this is equal to what we want
            }
        }
        if(retVal == STATS_OPERATION_INCOMPLETE){
            retVal = STATS_OPERATION_OK;
        }
    } else if(retVal == STATS_OPERATION_INCOMPLETE) {
        retVal = STATS_OPERAITON_ERROR_INVALID_QUARTILE;
    }

    if(sortedData != data && sortedData != nullptr){
        delete sortedData;
    }

    return retVal;
}

StatsOperation CalculateMean(float* data, uint16_t len, float* mean, bool highPrecision, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    float* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new float[len];
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, SortData(data, sortedData, len));
        } else{
            sortedData = data;
        }
    }

    float q1, median, q3;

    if(retVal == STATS_OPERATION_INCOMPLETE){
        STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindQuartile(sortedData, len, 1, &q1, false));
        STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindMedian(sortedData, len, &median, false));
        STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindQuartile(sortedData, len, 3, &q3, false));
    }

    if(retVal == STATS_OPERATION_INCOMPLETE){
        float iqr = q3 - q1;

        float outlierLowerBound = q1 - iqr * 1.5f;
        float outlierUpperBound = q3 + iqr * 1.5f;

        if(highPrecision){
            double result = 0;
            uint16_t numItems = 0;
            for(int i = 0; i < len; i++){
                float currData = sortedData[i];
                if(currData > outlierLowerBound && currData < outlierUpperBound){
                    result += currData;
                    numItems++;
                }
            }
            result /= (float) numItems;
            *mean = result;
        } else{
            float result = 0;
            uint16_t numItems = 0;
            for(int i = 0; i < len; i++){
                float currData = sortedData[i];
                if(currData > outlierLowerBound && currData < outlierUpperBound){
                    result += currData;
                    numItems++;
                }
            }
            result /= (float) numItems;
            *mean = result;
        }

        retVal = STATS_OPERATION_OK;
    }


    if(sortedData != data && sortedData != nullptr){
        delete sortedData;
    }

    return retVal;
}

StatsOperation CalculateMean(int32_t* data, uint16_t len, int32_t* mean, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;

    return retVal;
}
