#include <fstream>
#include <string>

#include <gtest/gtest.h>

#include "tree.h"
#include "funcs.h"

TEST(test1, keys4_req3)
{
	test_funcs::run_test("/test1/keys4_req3");
}

TEST(test2, keys20_req10)
{
	test_funcs::run_test("/test2/keys20_req10");
}

TEST(test3, keys100_req10)
{
	test_funcs::run_test("/test3/keys100_req10");
}

TEST(test4, keys100000_req100)
{
	test_funcs::run_test("/test4/keys100000_req100");
}

TEST(test5, keys0_req2)
{
	test_funcs::run_test("/test5/keys0_req2");
}

TEST(test6, unit_test1)
{
	Trees::SearchTree<std::pair<int, int>> set {};

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

	EXPECT_TRUE (data.size() == set.size());
	auto itt_set = set.begin();

	for (auto itt = data.begin(), end = data.end() ; itt != end; ++itt, ++itt_set)
	{
		EXPECT_EQ ((*itt).first,  (*itt_set).first);
		EXPECT_EQ ((*itt).second, (*itt_set).second);
	}
}
