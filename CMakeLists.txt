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

add_subdirectory(riscv)
