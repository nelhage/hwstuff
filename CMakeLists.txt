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
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_subdirectory(third_party/fmt-10.0.0 EXCLUDE_FROM_ALL)

add_executable(
  riscv_test
  riscv/test/alu_test.cc
)
verilate(riscv_test
  SOURCES riscv/alu.sv
)
verilate(riscv_test
  SOURCES riscv/regfile.sv
)

enable_testing()

target_link_libraries(
  riscv_test
  PUBLIC GTest::gtest_main
)

target_link_libraries(
  riscv_test
  PRIVATE fmt::fmt
)

include(GoogleTest)
gtest_discover_tests(riscv_test)

add_custom_target(build_and_test
  COMMAND env CTEST_OUTPUT_ON_FAILURE=1 ${CMAKE_CTEST_COMMAND}
  DEPENDS riscv_test
)
