#include <algorithm>
#include <cassert>
#include <cstring>
#include <iostream>
#include <stdint.h>
#include <sys/time.h>
#include <vector>

size_t getRadix(int val, int num) {
	return (val >> (8 * num)) & 0xFF;
}

size_t getBitPos(uint32_t n) {
   size_t pos = 0;
   if(n & 0xFFFF0000) pos = 16, n >>= 16;
   if(n & 0x0000FF00) pos += 8, n >>= 8;
   if(n & 0x000000F0) pos += 4, n >>= 4;
   if(n & 0x0000000C) pos += 2, n >>= 2;
   if(n & 0x00000002) pos += 1;
   return pos;
}

size_t getRandValue() {
    int result = 0;

    result |= (rand() % 255) << 0;
    result |= (rand() % 255) << 8;
    result |= (rand() % 255) << 16;
    result |= (rand() % 255) << 24;

    return result;
}

template <typename Collection>
void radixSort(Collection& array) {
    typedef typename Collection::value_type ValueType;

    const size_t RADIX = 4;
    const size_t COUNTS_SIZE = 0x101;
    const size_t ARRAY_SIZE = array.size();
    const size_t MAX_STACK_ARRAY_SIZE = 0xFF;

    const size_t LARGEST_ALIQUOT4 = ARRAY_SIZE & ~3;
    const size_t REST_SIZE = ARRAY_SIZE - LARGEST_ALIQUOT4;

    ValueType stackTmpArray[MAX_STACK_ARRAY_SIZE];
    bool useArrayInHeap = false;

    ValueType* tmpArray = 0;
    if (ARRAY_SIZE <= MAX_STACK_ARRAY_SIZE) {
        tmpArray = stackTmpArray;
    } else {
        tmpArray = new ValueType[ARRAY_SIZE];
        useArrayInHeap = true;
    }

    ValueType* sorted = &array.front();
    ValueType* buffer = tmpArray;

    uint32_t counts[COUNTS_SIZE * RADIX];
    memset(counts, 0, sizeof(uint32_t) * COUNTS_SIZE * RADIX);

    uint32_t* currentCounts = counts;
    uint32_t* nextCounts = counts + COUNTS_SIZE;

    for (size_t i = 0; i < LARGEST_ALIQUOT4;) {
        ++(currentCounts[getRadix(sorted[i++], 0) + 1]);
        ++(currentCounts[getRadix(sorted[i++], 0) + 1]);
        ++(currentCounts[getRadix(sorted[i++], 0) + 1]);
        ++(currentCounts[getRadix(sorted[i++], 0) + 1]);
    }

    for (size_t i = LARGEST_ALIQUOT4; i < ARRAY_SIZE; ++i) {
        ++(currentCounts[getRadix(sorted[i], 0) + 1]);
    }

    for (size_t i = 1; i < COUNTS_SIZE;) {
        currentCounts[i++] += currentCounts[i - 1];
        currentCounts[i++] += currentCounts[i - 1];
        currentCounts[i++] += currentCounts[i - 1];
        currentCounts[i++] += currentCounts[i - 1];
    }

    size_t tmpArrayIndex = 0;
    size_t countsIndex = 0;
    ValueType* value = 0;
    for (uint8_t r = 0; r < RADIX - 1; ++r) {
        const ValueType * const firstLast = sorted + LARGEST_ALIQUOT4;
        value = sorted;
        for (; value < firstLast; ) {
            tmpArrayIndex = (currentCounts[getRadix(*value, r)])++;
            buffer[tmpArrayIndex] = *value;
            countsIndex = getRadix(buffer[tmpArrayIndex], r + 1) + 1;
            ++(nextCounts[countsIndex]);

            ++value;

            tmpArrayIndex = (currentCounts[getRadix(*value, r)])++;
            buffer[tmpArrayIndex] = *value;
            countsIndex = getRadix(buffer[tmpArrayIndex], r + 1) + 1;
            ++(nextCounts[countsIndex]);

            ++value;

            tmpArrayIndex = (currentCounts[getRadix(*value, r)])++;
            buffer[tmpArrayIndex] = *value;
            countsIndex = getRadix(buffer[tmpArrayIndex], r + 1) + 1;
            ++(nextCounts[countsIndex]);

            ++value;

            tmpArrayIndex = (currentCounts[getRadix(*value, r)])++;
            buffer[tmpArrayIndex] = *value;
            countsIndex = getRadix(buffer[tmpArrayIndex], r + 1) + 1;
            ++(nextCounts[countsIndex]);

            ++value;
        }

        const ValueType * const secondLast = sorted + ARRAY_SIZE;
        for (; value < secondLast; ) {
            tmpArrayIndex = (currentCounts[getRadix(*value, r)])++;
            buffer[tmpArrayIndex] = *value;

            ++(nextCounts[getRadix(buffer[tmpArrayIndex], r + 1) + 1]);

            ++value;
        }

        for (size_t i = 1; i < COUNTS_SIZE;) {
            nextCounts[i++] += nextCounts[i - 1];
            nextCounts[i++] += nextCounts[i - 1];
            nextCounts[i++] += nextCounts[i - 1];
            nextCounts[i++] += nextCounts[i - 1];
        }

        currentCounts += COUNTS_SIZE;
        nextCounts += COUNTS_SIZE;
        std::swap(sorted, buffer);
    }

    for (size_t i = 0; i < ARRAY_SIZE; ++i) {
        buffer[(currentCounts[getRadix(sorted[i], RADIX - 1)])++] = sorted[i];
    }

    if (useArrayInHeap) {
        delete [] tmpArray;
    }
}

std::pair<float, float> evaluate(size_t size) {
    float radixTime = .0;
    float qsortTime = .0;

    float predRadixTime = .0;
    float predQsortTime = .0;

    struct timeval start, end;

    typedef unsigned long ValueType;

    const size_t SIZE = size;
    for (size_t i = 0; i < 10000; ++i) {
        std::vector<ValueType> vec1(SIZE);
        std::vector<ValueType> vec2(SIZE);
        for (size_t i = 0; i < SIZE; ++i) {
            ValueType value = getRandValue();
            vec1[i] = value;
            vec2[i] = value;
        }

        gettimeofday(&start, NULL);
        radixSort(vec1);
        gettimeofday(&end, NULL);

        int seconds  = end.tv_sec  - start.tv_sec;
        int useconds = end.tv_usec - start.tv_usec;

        predRadixTime = radixTime;
        radixTime += ((seconds) * 1000 + useconds/1000.0);

        assert(predRadixTime <= radixTime);

        gettimeofday(&start, NULL);
        std::sort(vec2.begin(), vec2.end());
        gettimeofday(&end, NULL);

        seconds  = end.tv_sec  - start.tv_sec;
        useconds = end.tv_usec - start.tv_usec;

        predQsortTime = qsortTime;
        qsortTime += ((seconds) * 1000 + useconds/1000.0);

        assert(predQsortTime <= qsortTime);

        assert(vec1 == vec2);
    }

    return std::make_pair(radixTime, qsortTime);
}

int main(int argc, char* argv[]) {
    for (size_t i = 2; i <= 100; ++i) {
        std::pair<float, float> result = evaluate(i);
        std::cout << i << " " << result.first << " " << result.second << "\n";
        if (result.first < result.second) {
            break;
        }
    }

    return 0;
}
