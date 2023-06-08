set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(
  riscv_test
  test/alu_test.cc
  test/memory_test.cc
  test/hart_test.cc
)
verilate(riscv_test
  SOURCES alu.sv
  VERILATOR_ARGS -Wall
)
verilate(riscv_test
  SOURCES regfile.sv
  VERILATOR_ARGS -Wall
)
verilate(riscv_test
  SOURCES memory.sv
  VERILATOR_ARGS -Wall
)
verilate(riscv_test
  SOURCES hart.sv
  VERILATOR_ARGS -Wall
)


target_link_libraries(
  riscv_test
  PUBLIC GTest::gtest_main
)

target_link_libraries(
  riscv_test
  PRIVATE fmt::fmt
)
