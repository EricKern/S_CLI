#pragma once
#include <iostream>
#include <vector>
#include <regex>
#include <string>

enum RangeType { DenseRange, Range, Unknown };

std::vector<int64_t> generate_range(int64_t start, int64_t end, int64_t step, RangeType range_type) {
    std::vector<int64_t> range;
    int64_t value = start;

    if (range_type == DenseRange) {
        while (value < end) {
            range.push_back(value);
            value += step;
        }
    } else if (range_type == Range) {
        while (value < end) {
            range.push_back(value);
            value *= step;
        }
    }

    return range;
}

RangeType detect_range_type(const std::string& input) {
    if (input.find("DenseRange") != std::string::npos) {
        return DenseRange;
    } else if (input.find("Range") != std::string::npos) {
        return Range;
    }
    return Unknown;
}

std::vector<int64_t> parse_range(int argc, char* argv[], const char* identifier) {
    std::regex pattern("--" + std::string(identifier) + "=(DenseRange|Range)\\[(\\d+),(\\d+),(\\d+)\\]");
    std::smatch matches;

    for (int i = 1; i < argc; ++i) {
        std::string input(argv[i]);
        if (std::regex_search(input, matches, pattern) && matches.size() == 5) {
            RangeType range_type = detect_range_type(matches[1]);
            if (range_type == Unknown) {
                continue;
            }

            int64_t start = std::stol(matches[2]);
            int64_t end = std::stol(matches[3]);
            int64_t step = std::stol(matches[4]);

            return generate_range(start, end, step, range_type);
        }
    }
    return {};
}

template <typename T>
bool parse_value(T* value, const char* identifier, int argc, char* argv[]) {
    std::regex pattern("--" + std::string(identifier) + "=(\\S+)");
    std::smatch matches;

    for (int i = 1; i < argc; ++i) {
        std::string input(argv[i]);
        if (std::regex_search(input, matches, pattern) && matches.size() == 2) {
            std::istringstream iss(matches[1]);
            iss >> *value;
            return true;
        }
    }
    return false;
}

// Specialization for const char*
template <>
bool parse_value<const char*>(const char** value, const char* identifier, int argc, char* argv[]) {
    std::regex pattern("--" + std::string(identifier) + "=(\\S+)");
    std::smatch matches;

    for (int i = 1; i < argc; ++i) {
        std::string input(argv[i]);
        if (std::regex_search(input, matches, pattern) && matches.size() == 2) {
            *value = argv[i] + input.find('=') + 1;
            return true;
        }
    }
    return false;
}

/*
./example.out --w=3 --blocks=DenseRange[2,7,2] --threads=Range[2,17,2]

int main(int argc, char* argv[]) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0] << " --threads=DenseRange[start, end, step] --blocks=Range[start, end, multiplier]" << std::endl;
        return 1;
    }

    std::vector<int64_t> threads_range = parse_range(argc, argv, "threads");
    std::vector<int64_t> blocks_range = parse_range(argc, argv, "blocks");

    if (threads_range.empty() || blocks_range.empty()) {
        std::cerr << "Invalid input format!" << std::endl;
        return 1;
    }

    std::cout << "Threads range: ";
    for (int value : threads_range) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    std::cout << "Blocks range: ";
    for (int value : blocks_range) {
        std::cout << value << " ";
    }
    std::cout << std::endl;

    const char* text = nullptr;;
    parse_value(&text, "w", argc, argv);

    std::cout << "Warmups: " << text << std::endl;

    return 0;
}
*/
