#pragma once

#include <fstream>
#include <iostream>
#include <stdexcept>
#include <utility>
#include <vector>

namespace get_info {
void handler(char id, std::istream &stream, std::vector<int> &buff, Trees::SearchTree<int> &set) {
    if (id == 'k') {
        int key = 0;
        if (!(stream >> key)) throw std::runtime_error("incorrect key");
        set.insert(key);
    } else if (id == 'q') {
        int first = 0;
        int second = 0;

        if (!(stream >> first >> second))
            throw std::runtime_error("incorrect border value in request");

        if (set.empty()) {
            buff.push_back(0);
            return;
        } else if (first > second) {
            buff.push_back(0);
            return;
        }

        auto res = std::distance(set.lower_bound(first), set.upper_bound(second));
        buff.push_back(res);
    } else {
        throw std::runtime_error("unknown id");
    }
}
}  // namespace get_info
