set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
  riscv_test
  test/alu_test.cc
  test/memory_test.cc
)
verilate(riscv_test
  SOURCES alu.sv
)
verilate(riscv_test
  SOURCES regfile.sv
)
verilate(riscv_test
  SOURCES memory.sv
)


target_link_libraries(
  riscv_test
  PUBLIC GTest::gtest_main
)

target_link_libraries(
  riscv_test
  PRIVATE fmt::fmt
)
