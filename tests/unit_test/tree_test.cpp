#include "tree.h"

#include <gtest/gtest.h>

#include <fstream>
#include <string>

TEST(tree, emplace) {
    Trees::SearchTree<std::pair<int, int>> set{};

    set.emplace(1, 2);
    set.emplace(3, 4);
    set.emplace(5, 6);

    std::vector<std::pair<int, int>> data = {};

    std::pair<int, int> p1 = {1, 2};
    std::pair<int, int> p2 = {3, 4};
    std::pair<int, int> p3 = {5, 6};

    data.push_back(p1);
    data.push_back(p2);
    data.push_back(p3);

    EXPECT_TRUE(data.size() == set.size());
    auto itt_set = set.begin();

    for (auto itt = data.begin(), end = data.end(); itt != end; ++itt, ++itt_set) {
        EXPECT_EQ((*itt).first, (*itt_set).first);
        EXPECT_EQ((*itt).second, (*itt_set).second);
    }
}

TEST(tree, copy_ctor) {
    Trees::SearchTree<int> tree = {};

    tree.insert(1);
    tree.insert(2);
    tree.insert(3);
    tree.insert(4);
    tree.insert(5);

    Trees::SearchTree<int> lhs = tree;

    EXPECT_TRUE(tree.size() == lhs.size());

    for (auto itt = tree.begin(), itt_lhs = lhs.begin(), end = tree.end(); itt != end;
         ++itt, ++itt_lhs) {
        EXPECT_EQ(*itt, *itt_lhs);
    }
}
