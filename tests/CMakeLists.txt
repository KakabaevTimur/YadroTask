include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        release-1.11.0
)
FetchContent_MakeAvailable(googletest)
add_library(GTest::GTest INTERFACE IMPORTED)
target_link_libraries(GTest::GTest INTERFACE gtest_main)

add_executable(test test.cpp ${PROJECT_SOURCE_DIR}/room.cpp)

target_link_libraries(test PRIVATE GTest::GTest)

target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR})
