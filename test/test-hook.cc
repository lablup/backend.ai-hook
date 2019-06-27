#include "utils.hh"
#include "patch-libs.hh"

#include "gtest/gtest.h"

#include <iostream>
#include <fstream>

namespace {

TEST(has_prefix, basic) {
    EXPECT_TRUE(has_prefix("a", "abc"));
    EXPECT_TRUE(has_prefix("", ""));
    EXPECT_FALSE(has_prefix("a", "bbc"));
}

TEST(cpuset_parsing, basic) {
    int n;
    std::string tmp_path = std::tmpnam(nullptr);
    std::ofstream tmp_file;
    std::ios_base::openmode mode = std::ofstream::out | std::ofstream::trunc;

    tmp_file.open(tmp_path, mode);
    tmp_file << "0";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 1);

    tmp_file.open(tmp_path, mode);
    tmp_file << "0-1";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 2);

    tmp_file.open(tmp_path, mode);
    tmp_file << "0-5";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 6);

    tmp_file.open(tmp_path, mode);
    tmp_file << "0,1";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 2);

    tmp_file.open(tmp_path, mode);
    tmp_file << "9,1,2,8";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 4);

    tmp_file.open(tmp_path, mode);
    tmp_file << "0,5-8";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 5);

    tmp_file.open(tmp_path, mode);
    tmp_file << "60-62,99-100";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 5);

    tmp_file.open(tmp_path, mode);
    tmp_file << "0,2-3,5,7-10";
    tmp_file.close();
    n = get_num_processors_from_cpuset(tmp_path.c_str());
    EXPECT_EQ(n, 8);

    std::remove(tmp_path.c_str());
}

}
