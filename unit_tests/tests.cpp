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
