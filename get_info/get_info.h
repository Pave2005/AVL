#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <utility>

namespace get_info
{
    void handler (char id, std::istream& stream, std::vector<int>& buff, Trees::SearchTree<int>& set)
    {
        if (id == 'k')
        {
            int key = 0;
            stream >> key;
            set.insert (key);
        }
        else if (id == 'q')
        {
            int first = 0;
            int second = 0;

            stream >> first >> second;

            if (first > second) throw "The left border can not be greater then right";

            auto res = std::distance(set.lower_bound (first), set.upper_bound (second));
            buff.push_back(res);
        }
        else
        {
            throw "unknown id";
        }
    }
}
