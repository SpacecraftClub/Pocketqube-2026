#include "stats.h"


// for some reason this includes even when running on ra4m1?
// #if RUNNING_WIN_NATIVE
// #include <unity.h>
// #endif

/*
NF 1-29-2026
BYU Spacecraft Club
Stats Library for cleaning data

TODO:
Unit Tests
*/

#pragma message "Stats.cpp compiled"

#define IS_INT_ODD(num) ((num % 2) != 0)
#define STATS_OPERATION_CONCATINATE_RESULTS(val, fn) val = ((StatsOperation)((val) | (fn)))

/*
These next few templates (is_same, conditional) were part of the AI magic for the cpp compiler 
because I wanted the int32_t declaration of the function to be exactly the same except for the 
data type used for summing it up, wheras I didn't want it type T, rather int64_t.
*/
template <typename T, typename U>
struct is_same {
    static const bool value = false;
};

template <typename T>
struct is_same<T, T> {
    static const bool value = true;
};

template <bool B, typename TrueType, typename FalseType>
struct conditional {
    using type = TrueType;
};

template <typename TrueType, typename FalseType>
struct conditional<false, TrueType, FalseType> {
    using type = FalseType;
};


template<typename T>
StatsOperation SortData(T* data, T* output, uint16_t len){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }
    
    if(retVal == STATS_OPERATION_INCOMPLETE){
        output[0] = data[0];
        for(int32_t i = 1; i < len; i++){
            if(data[i] < output[i-1]){
                for(int32_t j = i - 1; j >= 0; j--){
                    if(data[i] < output[j]){
                        output[j+1] = output[j];
                        if(j == 0){
                            output[j] = data[i];
                        }
                    } else{
                        output[j+1] = data[i];
                        break;
                    }
                }
            } else {
                output[i] = data[i];
            }
        }
        retVal = STATS_OPERATION_OK;
    }


    return retVal;
}

template StatsOperation SortData<int32_t>(int32_t*, int32_t*, uint16_t);
template StatsOperation SortData<float>(float*, float*, uint16_t);


template<typename T>
StatsOperation FindMedian(T* data, uint16_t len, T* median, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;

    using AccumType = typename conditional<is_same<T, int32_t>::value, int64_t, T>::type;

    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    T* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new T[len];
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, SortData(data, sortedData, len));
        } else{
            sortedData = data;
        }
    }

    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(IS_INT_ODD(len)){
            *median = sortedData[len / 2]; // get the middle index
        } else{
            AccumType sum;
            sum = sortedData[len / 2 - 1];
            sum += sortedData[len/2];
            *median = (T) ((AccumType) sum / (AccumType) 2);
        }
        retVal = STATS_OPERATION_OK;
    }

    if(sortedData != data && sortedData != nullptr){
        delete sortedData;
    }

    return retVal;
}

template StatsOperation FindMedian<int32_t>(int32_t*, uint16_t, int32_t*, bool);
template StatsOperation FindMedian<float>(float*, uint16_t, float*, bool);


template<typename T>
StatsOperation FindQuartile(T* data, uint16_t len, uint16_t quartileNumber, T* quartileValue, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;
    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    T* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new T[len];
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
                FindMedian(sortedData, medianIndex , quartileValue, false); // this is equal to what we want
            } else{ // we know that quartileNumber = 3
                FindMedian(sortedData + medianIndex + (IS_INT_ODD(len) ? 1: 0), len - medianIndex - (IS_INT_ODD(len) ? 1: 0), quartileValue, false); // this is equal to what we want
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

template StatsOperation FindQuartile<int32_t>(int32_t*, uint16_t, uint16_t , int32_t*, bool);
template StatsOperation FindQuartile<float>(float*, uint16_t, uint16_t , float*, bool);


template<typename T>
StatsOperation CalculateMean(T* data, uint16_t len, T* mean, bool highPrecision, bool shouldSortFirst){
    StatsOperation retVal = STATS_OPERATION_INCOMPLETE;

    //This next line is from some AI voodo magic in the CPP compiler so that the sum datatype for int32_t is int64_t wheras for floats it will be just floats
    using AccumType = typename conditional<is_same<T, int32_t>::value, int64_t, T>::type;

    if(data == nullptr){
        retVal = STATS_OPERATION_ERROR_NULL_PTR;
    }

    T* sortedData = nullptr;
    if(retVal == STATS_OPERATION_INCOMPLETE){
        if(shouldSortFirst){
            sortedData = new T[len];
            STATS_OPERATION_CONCATINATE_RESULTS(retVal, SortData(data, sortedData, len));
        } else{
            sortedData = data;
        }
    }

    T q1, median, q3;

    if(retVal == STATS_OPERATION_INCOMPLETE){
        STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindQuartile(sortedData, len, 1, &q1, false));
        STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindMedian(sortedData, len, &median, false));
        STATS_OPERATION_CONCATINATE_RESULTS(retVal, FindQuartile(sortedData, len, 3, &q3, false));
    }

    if(retVal == STATS_OPERATION_INCOMPLETE){
        AccumType iqr = q3 - q1;
        AccumType outlierRangeFromQuartile = iqr * 1.5f;

        AccumType outlierLowerBound = (AccumType) q1 - (AccumType) outlierRangeFromQuartile;
        AccumType outlierUpperBound = (AccumType) q3 + (AccumType) outlierRangeFromQuartile;

        if(highPrecision){
            double result = 0;
            uint16_t numItems = 0;
            for(int i = 0; i < len; i++){
                T currData = sortedData[i];
                if(currData > outlierLowerBound && currData < outlierUpperBound){
                    result += currData;
                    numItems++;
                }
            }
            result /= (T) numItems;
            *mean = result;
            retVal = STATS_OPERATION_OK;
        } else{
            AccumType result = 0;
            uint16_t numItems = 0;
            for(int i = 0; i < len; i++){
                T currData = sortedData[i];
                if((AccumType) currData > outlierLowerBound && (AccumType) currData < outlierUpperBound){
                    result += currData;
                    numItems++;
                }
            }
            if(numItems != 0){
                result = result / numItems;
                *mean = result;
                retVal = STATS_OPERATION_OK;
            } else{
                retVal = STATS_OPERATION_ERROR_UNKNOWN;
            }
        }

    }


    if(sortedData != data && sortedData != nullptr){
        delete sortedData;
    }

    return retVal;
}

template StatsOperation CalculateMean<int32_t>(int32_t*, uint16_t, int32_t*, bool, bool);
template StatsOperation CalculateMean<float>(float*, uint16_t, float*, bool, bool);