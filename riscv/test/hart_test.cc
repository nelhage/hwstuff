#include <gtest/gtest.h>

#include "Vhart.h"
#include "Vhart_hart.h"
#include "Vhart_regfile.h"

#include "svdpi.h"
#include <verilated.h>

static void nomem(){};

template <typename T = typeof(nomem)>
void run_insn(Vhart &hart, uint32_t insn, T&& handle_mem = nomem) {
  hart.clk = 0;
  hart.insn = insn;
  hart.eval();
  handle_mem();
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

  // c:   fbcd02b7                lui     t0,0xfbcd0
  run_insn(hart, 0xfbcd02b7);
  EXPECT_EQ(hart.hart->regs->rf[5], 0xfbcd0000);

  // 10:   000041b7                lui     gp,0x4
  run_insn(hart, 0x000041b7);
  EXPECT_EQ(hart.hart->regs->rf[3], 0x4000);

  // 14:   1231a583                lw      a1,0x123(gp)
  run_insn(hart, 0x1231a583, [&hart]() {
    EXPECT_EQ(hart.memaddr, 0x4123);
    EXPECT_EQ(hart.memw, 0);
    EXPECT_EQ(hart.memsext, 1);
    EXPECT_EQ(hart.memwidth, 2);
    hart.memrdata = 0xf00fbeef;
  });
  EXPECT_EQ(hart.hart->regs->rf[11], 0xf00fbeef);

  // 18:   12318583                lb      a1,0x123(gp)
  run_insn(hart, 0x12318583, [&hart]() {
    EXPECT_EQ(hart.memaddr, 0x4123);
    EXPECT_EQ(hart.memw, 0);
    EXPECT_EQ(hart.memsext, 1);
    EXPECT_EQ(hart.memwidth, 0);
    hart.memrdata = 0xffffff71;
  });
  EXPECT_EQ(hart.hart->regs->rf[11], 0xffffff71);
  // 1c:   1231c583                lbu     a1,0x123(gp)
  run_insn(hart, 0x1231c583, [&hart]() {
    EXPECT_EQ(hart.memaddr, 0x4123);
    EXPECT_EQ(hart.memw, 0);
    EXPECT_EQ(hart.memsext, 0);
    EXPECT_EQ(hart.memwidth, 0);
    hart.memrdata = 0x71;
  });
  EXPECT_EQ(hart.hart->regs->rf[11], 0x71);

  // 28:   00444617                auipc   x12,0x444
  hart.pc = 0x20000000;
  run_insn(hart, 0x00444617);
  EXPECT_EQ(hart.hart->regs->rf[12], 0x20444000);

  // 2c:   0bf625a3                sw      x31,0xab(x12)
  hart.hart->regs->rf[31] = 0x4321abcd;
  run_insn(hart, 0x0bf625a3, [&hart]() {
    EXPECT_EQ(hart.memaddr, 0x204440ab);
    EXPECT_EQ(hart.memw, 1);
    EXPECT_EQ(hart.memwidth, 2);
    EXPECT_EQ(hart.memwdata, 0x4321abcd);
  });
  // 30:   0bf605a3                sb      x31,0ax1b(x12)
  run_insn(hart, 0x0bf605a3, [&hart]() {
    EXPECT_EQ(hart.memaddr, 0x204440ab);
    EXPECT_EQ(hart.memw, 1);
    EXPECT_EQ(hart.memwidth, 0);
    EXPECT_EQ(hart.memwdata, 0x4321abcd);
  });

  // 34:   1850056f                jal     x10,.+0x984
  hart.pc = 0x121212;
  run_insn(hart, 0x1850056f);
  EXPECT_EQ(hart.hart->regs->rf[10], 0x121212+4);
  EXPECT_EQ(hart.nextpc, 0x121212+0x984);

  // 38:   00b98567                jalr    x10,11(x19)
  hart.pc = 0xd00db0d;
  hart.hart->regs->rf[19] = 0xcafef00d;
  run_insn(hart, 0x00b98567);
  EXPECT_EQ(hart.hart->regs->rf[10], 0xd00db0d+4);
  EXPECT_EQ(hart.nextpc, 0xcafef00d+11);

  // Branches


  hart.final();
}
