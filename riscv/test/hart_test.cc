#include <gtest/gtest.h>

#include "Vhart.h"
#include "Vhart_hart.h"
#include "Vhart_regfile.h"

#include "svdpi.h"
#include <verilated.h>

void run_insn(Vhart &hart, uint32_t insn) {
  hart.clk = 0;
  hart.insn = insn;
  hart.eval();
  hart.clk = 1;
  hart.eval();
}

TEST(RISCVTest, HartTest) {
  auto contextp = std::make_unique<VerilatedContext>();
  Vhart hart;

  hart.clk = 0;
  hart.reset = 0;
  hart.eval();
  hart.reset = 1;
  hart.clk = 1;
  hart.eval();
  hart.reset = 0;

  for (auto i = 0; i < 32; i++) {
    EXPECT_EQ(hart.hart->regs->rf[10], 0) << "reg " << i << "@reset";
  }

  // 0:   02c00513                li      a0,44
  // 4:   07b00293                li      t0,123
  // 8:   00a28333                add     t1,t0,a0

  run_insn(hart, 0x02c00513);
  EXPECT_EQ(hart.hart->regs->rf[10], 44);
  run_insn(hart, 0x07b00293);
  EXPECT_EQ(hart.hart->regs->rf[5], 123);
  run_insn(hart, 0x00a28333);
  EXPECT_EQ(hart.hart->regs->rf[6], 44+123);


  hart.final();
}
