#include <gtest/gtest.h>

#include <room.hpp>

TEST(Tests, TestExample)
{
    testing::internal::CaptureStdout();
    {
        Room r{std::filesystem::current_path() / "tests" / "in_files" / "example.txt"};
        r.start();
    }
    std::string output = testing::internal::GetCapturedStdout();

    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "in_files" / "example.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}
