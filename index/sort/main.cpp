#include <iostream>

#include <data.h>
#include <sorter.h>

namespace {

void printUsage() {
    std::cout << "Usage: sort_file data_file_name tmp_data_dir items_in_chunk thread_count out_file_name\n";
}

struct EventCallback {
    void operator()(SortEventType type, int param) {
        switch (type) {
            case BeginCreatingChunks:
                std::cout << "Creating chunks..." << "\n";
                break;
            case DoneCreatingChunks:
                std::cout << param << " chunks created\n";
                break;
            case BeginSortingChunks:
                std::cout << "Sorting chunks..." << "\n";
                break;
            case ChunkSorted:
                std::cout << "Chunk " << param << " sorted\n";
                break;
            case EndSortingChunks:
                std::cout << "Done" << "\n";
                break;
            case BeginMergingChunks:
                std::cout << "Merging chunks..." << "\n";
                break;
            case DoneMergingChunks:
                std::cout << "Done" << "\n";
                break;
        }
    }
};

struct Sort {
    template <typename RandomAccessIterator>
    void operator()(RandomAccessIterator begin, RandomAccessIterator end) {
        std::sort(begin, end);
    }
};

}

int main(int argc, char* argv[]) {
    if (argc != 6) {
        printUsage();
        return 1;
    }

    const char* dataFileName = argv[1];
    const char* chunkDir = argv[2];
    size_t itemsInChunk = atoi(argv[3]);
    size_t threadCount = atoi(argv[4]);
    const char* outputFileName = argv[5];

    try {
        externalSort<DataEntry>(dataFileName, chunkDir, outputFileName,
                itemsInChunk, threadCount, Sort(), EventCallback());
    } catch (std::exception& ex) {
        std::cerr << ex.what() << "\n";
        return 1;
    }

    return 0;
}
