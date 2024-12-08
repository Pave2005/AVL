#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <string_view>

#include <gtest/gtest.h>

#include "tree.h"
#include "get_info.h"

namespace test_funcs
{
    const int error_id = -1;

	void get_result (std::string_view filename, std::vector<int>& res)
    {
        std::ifstream file(filename.data());
        if (!file)
        {
            throw std::runtime_error("file error");
        }

        Trees::SearchTree<int> set{};

        char id = 0;
        while (file >> id)
        {
            get_info::handler (id, file, res, set);
        }

        file.close();
    }

    void get_answer(std::string_view filename, std::vector<int>& ans)
    {
        std::ifstream answer_file(filename.data());

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

	void run_test (std::string_view test_name)
	{
        try
        {
            std::string test_directory = "/tests";

            std::string test_path = std::string(TEST_DATA_DIR) + test_directory;
            test_path.append(test_name);

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
        catch (const char* error_message)
        {
            std::cout << error_message << std::endl;
            exit (1);
        }
	}
}
