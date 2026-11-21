find_package(GTest REQUIRED)
include(GoogleTest)

option(GRITTY_SANITIZERS    "Build tests with ASan + UBSan" OFF)
option(GRITTY_NO_EXCEPTIONS "Build tests with -fno-exceptions" OFF)

add_executable(gritty_tests test_smoke.cpp)
target_link_libraries(gritty_tests PRIVATE gritty::gritty GTest::gtest_main)
target_compile_options(gritty_tests PRIVATE -Wall -Wextra -Wpedantic -Wconversion -Werror)

if(GRITTY_SANITIZERS)
  target_compile_options(gritty_tests PRIVATE -fsanitize=address,undefined -fno-omit-frame-pointer)
  target_link_options(gritty_tests PRIVATE -fsanitize=address,undefined)
endif()
if(GRITTY_NO_EXCEPTIONS)
  target_compile_options(gritty_tests PRIVATE -fno-exceptions)
endif()

gtest_discover_tests(gritty_tests)
