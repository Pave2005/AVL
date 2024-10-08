#include "tree.h"
#include "get_info.h"

int main ()
{
    Trees::SearchTree<int> set{};

    std::vector<int> buff{};

    get_info::get_keys (std::cin, buff);

    set = { buff };

    std::vector<std::pair<int, int>> requests{};

    try
    {
        get_info::get_requests (std::cin, requests);

        for (auto request : requests)
        {
            auto res = std::distance (set.lower_bound(request.first), set.upper_bound (request.second));
            std::cout << res << " ";
        }
        std::cout << std::endl;
    }
    catch (const char* error_message)
    {
        std::cout << error_message << std::endl;
        exit (1);
    }

    #ifdef PIC
        set.tree_dump("dot.txt");
    #endif
}
