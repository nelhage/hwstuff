#include <cstdint>
#include <gtest/gtest.h>

#include "Vhart.h"
#include "Vhart_hart.h"
#include "Vhart_regfile.h"

#include "svdpi.h"
#include "opcodes.h"
#include <verilated.h>

#include <fmt/core.h>

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

void test_branch(Vhart &hart,
                 uint32_t branch_op,
                 uint32_t lhs,
                 uint32_t rhs,
                 bool expect_branch) {

  uint32_t pc = 0x19992023;
  uint32_t ra = 20;
  uint32_t rb = 22;
  int32_t delta = -130;
  uint32_t delta_imm = (uint32_t)(delta & 0x1fff);

  uint32_t insn = (
                   (((delta_imm >> 12) & 0b1) << 31) |
                   (((delta_imm >> 5) & 0b111111) << 25) |
                   (rb << 20) |
                   (ra << 15) |
                   (branch_op << 12) |
                   (((delta_imm >> 1) & 0b1111) << 8) |
                   (((delta_imm >> 11) & 0b1) << 7) |
                   0x63
                   );

  hart.pc = pc;
  hart.hart->regs->rf[ra] = lhs;
  hart.hart->regs->rf[rb] = rhs;
  run_insn(hart, insn);

  uint32_t expectpc = expect_branch ? uint32_t(int32_t(pc) + delta) : pc + 4;

  EXPECT_EQ(hart.nextpc, expectpc)
    << fmt::format("pc={:08x} nextpc={:08x} pc+4={:08x} brpc={:08x} expect={}",
                   pc, hart.nextpc, pc+4, uint32_t(int32_t(pc) + delta), expect_branch);
}

TEST(HartTest, Smoke) {
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

  hart.pc = 0xd00db0d;
  hart.hart->regs->rf[19] = 0xcafef00e;
  run_insn(hart, 0x00b98567);
  EXPECT_EQ(hart.hart->regs->rf[10], 0xd00db0d+4);
  EXPECT_EQ(hart.nextpc, 0xcafef00d+11);

  // Conditional branches
  test_branch(hart, FUNCT3_BR_BEQ, 0x111, 0x111, true);
  test_branch(hart, FUNCT3_BR_BEQ, 0x111, 0x211, false);

  test_branch(hart, FUNCT3_BR_BNE, 0x111, 0x111, false);
  test_branch(hart, FUNCT3_BR_BNE, 0xaaa, 0x111, true);
  test_branch(hart, FUNCT3_BR_BNE, 0x110, 0x111, true);

  test_branch(hart, FUNCT3_BR_BLT, 0x110, 0x111, true);
  test_branch(hart, FUNCT3_BR_BLT, 0xcad, 0xbab, false);
  test_branch(hart, FUNCT3_BR_BLT, uint32_t(-1), 0, true);

  test_branch(hart, FUNCT3_BR_BLT, 0x110, 0x111, true);
  test_branch(hart, FUNCT3_BR_BLT, 0xeeabc, 0xeeabc, false);
  test_branch(hart, FUNCT3_BR_BLT, 0xcad, 0xbab, false);
  test_branch(hart, FUNCT3_BR_BLT, uint32_t(-1), 0, true);

  test_branch(hart, FUNCT3_BR_BGE, 0x110, 0x111, false);
  test_branch(hart, FUNCT3_BR_BGE, 0xeeabc, 0xeeabc, true);
  test_branch(hart, FUNCT3_BR_BGE, 0xcad, 0xbab, true);
  test_branch(hart, FUNCT3_BR_BGE, uint32_t(-1), 0, false);

  test_branch(hart, FUNCT3_BR_BLTU, 0x110, 0x111, true);
  test_branch(hart, FUNCT3_BR_BLTU, 0xeeabc, 0xeeabc, false);
  test_branch(hart, FUNCT3_BR_BLTU, 0xcad, 0xbab, false);
  test_branch(hart, FUNCT3_BR_BLTU, uint32_t(-1), 0, false);

  test_branch(hart, FUNCT3_BR_BGEU, 0x110, 0x111, false);
  test_branch(hart, FUNCT3_BR_BGEU, 0xeeabc, 0xeeabc, true);
  test_branch(hart, FUNCT3_BR_BGEU, 0xcad, 0xbab, true);
  test_branch(hart, FUNCT3_BR_BGEU, uint32_t(-1), 0, true);

  hart.final();
}
