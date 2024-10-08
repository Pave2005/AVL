#pragma once

#include <fstream>
#include <iostream>
#include <vector>
#include <utility>

namespace get_info
{
    void get_keys (std::istream& stream, std::vector<int>& buff)
    {
        int key_size = 0;

        stream >> key_size;

        buff.reserve(key_size);
        for (int i = 0; i < key_size; i++)
        {
            int key = 0;
            stream >> key;
            buff.push_back(key);
        }
    }

    void get_requests (std::istream& stream, std::vector<std::pair<int, int>>& buff)
    {
        int req_size = 0;

        stream >> req_size;

        buff.reserve(req_size);
        for (int i = 0; i < req_size; i++)
        {
            int fst = 0;
            int snd = 0;

            stream >> fst >> snd;

            if (fst > snd) { throw "The left border can not be greater then right"; }

            auto pair = std::make_pair (fst, snd);

            buff.push_back(pair);
        }
    }
}
