#include "tree.h"
#include <utility>

int main ()
{
    int elemCount = 30;
    Trees::SearchTree<int> set{};

    std::vector<int> buff{};
    buff.reserve(elemCount);

    for (int i = 0; i < elemCount; i++)
    {
        buff.push_back(rand () % elemCount);
    }

    set = { buff };

    int reqCount = 5;
    std::vector<std::pair<int, int>> requests{};

    for (int i = 0; i < reqCount; i++)
    {
        auto pair = std::make_pair (rand () % elemCount, rand () % elemCount);
        if (pair.first > pair.second)
        {
            std::swap (pair.first, pair.second);
        }
        requests.push_back(pair);
    }


    for (auto&& request : requests)
    {
        auto res = std::distance (set.lower_bound(request.first), set.upper_bound (request.second));
        std::cout << res << std::endl;
    }
}
