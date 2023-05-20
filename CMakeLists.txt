cmake_minimum_required(VERSION 3.22)
project(hwdev)
find_package(verilator HINTS $ENV{VERILATOR_ROOT})
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.13.0.zip
  DOWNLOAD_EXTRACT_TIMESTAMP true
  # https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)


add_executable(alu_test riscv/test/alu_test.cc)
verilate(alu_test SOURCES riscv/alu.sv)

enable_testing()

target_link_libraries(
  alu_test
  PUBLIC GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(alu_test)
