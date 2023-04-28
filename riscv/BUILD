load("@rules_verilator//verilator:defs.bzl", "sv_library", "verilator_cc_library")

sv_library(
    name = "alu_lib",
    srcs = ["alu.sv"],
)

verilator_cc_library(
    name = "alu",
    mtop = "alu",
    deps = [":alu_lib"],
)

cc_test(
    name = "alu_test",
    srcs = ["alu_test.cc"],
    deps = [":alu"],
)
