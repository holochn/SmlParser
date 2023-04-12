set(ThisTest "testSmlParser")

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)

FetchContent_MakeAvailable(googletest)

enable_testing()
add_executable(${ThisTest}
    testSmlParser.cpp
    ../src/SmlParser.cpp
)
target_include_directories(${ThisTest} PRIVATE
  ../src
)
target_link_libraries(
    ${ThisTest}
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(${ThisTest})
target_compile_options(${ThisTest} PRIVATE -Wall -Wextra -Wpedantic -Werror)
