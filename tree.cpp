#include "tree.h"
#include "get_info.h"

int main ()
try
{
    Trees::SearchTree<int> set{};

    std::vector<int> buff;

    char id = 0;
    while (std::cin >> id)
    {
        get_info::handler (id, std::cin, buff, set);
        if (id == 'q') std::cout << buff.back() << " ";
    }
    std::cout << std::endl;

    #ifdef PIC
        set.tree_dump("dot.txt");
    #endif
}
catch (std::exception& expt)
{
    std::cout << expt.what() << std::endl;
    exit (1);
}
