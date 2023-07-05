set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(modules_to_test alu memory hart fetchstore)

foreach(module IN LISTS modules_to_test)
  set(target "${module}_test")
  add_executable(
    "${target}"
    "test/${module}_test.cc"
    )
  verilate("${target}"
    SOURCES "${module}.sv"
    VERILATOR_ARGS -Wall
    )
  target_link_libraries(
    "${target}"
    PUBLIC GTest::gtest_main
    PRIVATE fmt::fmt
    )
  gtest_discover_tests(
    "${target}"
    )
endforeach()

