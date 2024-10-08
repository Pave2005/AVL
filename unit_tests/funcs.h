#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>

#include <gtest/gtest.h>

#include "tree.h"
#include "get_info.h"

namespace test_funcs
{
	void get_result (const std::string& filename, std::vector<int>& res)
    {
        std::ifstream file(filename);
        if (!file)
        {
            std::cout << "error\n";
            exit(1);
        }

        Trees::SearchTree<int> set{};

        std::vector<int> buff{};
        get_info::get_keys (file, buff);

        set = { buff };

        std::vector<std::pair<int, int>> requests{};
        get_info::get_requests (file, requests);

        res.reserve(requests.size());
        for (int i = 0; i < requests.size(); i++)
        {
            auto result = std::distance (set.lower_bound(requests[i].first), set.upper_bound (requests[i].second));
            res.push_back(result);
        }

        file.close();
    }

    void get_answer(const std::string& filename, std::vector<int>& ans)
    {
        std::ifstream answer_file(filename);

        int count = 0;
        answer_file >> count;

        ans.reserve(count);
        for (int i = 0; i < count; i++)
        {
            int answer = 0;
            answer_file >> answer;
            ans.push_back(answer);
        }

        answer_file.close();
    }

	void run_test (const std::string& test_name)
	{
		std::string test_directory = "/tests";

		std::string test_path = std::string(TEST_DATA_DIR) + test_directory + test_name;

        std::vector<int> res;
		get_result(test_path + ".dat", res);

        std::vector<int> ans;
		get_answer(test_path + ".ans", ans);

        EXPECT_TRUE(res.size() == ans.size());
        for (int i = 0; i < ans.size(); i++)
        {
            EXPECT_EQ(res[i], ans[i]);
        }
	}
}
