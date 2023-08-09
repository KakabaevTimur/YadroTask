#include <gtest/gtest.h>

#include <room.hpp>

TEST(Tests, TestFile)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{"badExample.txt"};
    }, std::runtime_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );

    EXPECT_STREQ("", output.c_str());
}

TEST(Tests, TestPositive)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    {
        Room r{std::filesystem::current_path() / "tests" / "files" / "example.txt"};
        r.start();
    }

    std::string output = buffer.str();
    std::cout.rdbuf( old );

    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "example.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}

TEST(Tests, TestWrongTablesCount)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{std::filesystem::current_path() / "tests" / "files" / "wrongTablesCount.txt"};
    }, std::format_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );
    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "wrongTablesCount.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}

TEST(Tests, TestWrongTime)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{std::filesystem::current_path() / "tests" / "files" / "wrongTime.txt"};
    }, std::format_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );
    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "wrongTime.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}

TEST(Tests, TestWrongStartTime)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{std::filesystem::current_path() / "tests" / "files" / "wrongStartTime.txt"};
    }, std::runtime_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );
    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "wrongStartTime.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}

TEST(Tests, TestWrongCost)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{std::filesystem::current_path() / "tests" / "files" / "wrongCost.txt"};
    }, std::format_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );
    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "wrongCost.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}

TEST(Tests, TestWithoutZero)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{std::filesystem::current_path() / "tests" / "files" / "withoutZero.txt"};
        r.start();
    }, std::format_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );
    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "withoutZero.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}

TEST(Tests, TestWrongEventTime)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{std::filesystem::current_path() / "tests" / "files" / "wrongEventTime.txt"};
        r.start();
    }, std::runtime_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );
    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "wrongEventTime.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}

TEST(Tests, TestWrongTableNumber)
{
    std::stringstream buffer;
    std::streambuf * old = std::cout.rdbuf(buffer.rdbuf());

    EXPECT_THROW({
        Room r{std::filesystem::current_path() / "tests" / "files" / "wrongTableNumber.txt"};
        r.start();
    }, std::runtime_error);

    std::string output = buffer.str();
    std::cout.rdbuf( old );
    std::ifstream expectedOutput{std::filesystem::current_path() / "tests" / "files" / "wrongTableNumber.out.txt"};
    char expected[1024] = {0};
    expectedOutput.read(expected, sizeof(expected));

    EXPECT_STREQ(expected, output.c_str());
}
