#include "Valu.h"
#include <verilated.h>

#include <gtest/gtest.h>

#include <fmt/core.h>

#include <memory>
#include <stdint.h>

using std::make_unique;

struct AluTest {
  uint32_t ctlhi : 1;
  uint32_t ctl : 3;
  uint32_t a, b;

  uint32_t expect;
};

TEST(RISCVTest, ALUTest) {
  auto contextp = make_unique<VerilatedContext>();

  Valu alu;

  AluTest test_cases[] = {
      // ADD
      {0, 0x0, 1234, 9823, 1234 + 9823},

      // SUB
      {1, 0x0, 1234, 9823, uint32_t(1234 - 9823)},
      {1, 0x0, 0, 1, 0xffffffff},
      {1, 0x0, 0xffffffff, 1, 0xfffffffe},
      {1, 0x0, 0, 0xffffffff, 1},

      {0, 0x2, 0, 1, 1},
      {0, 0x2, 1, 0, 0},
      {0, 0x2, 0xffffffff, 0, 1},
      {0, 0x2, 0, 0xffffffff, 0},

      {0, 0x3, 0, 1, 1},
      {0, 0x3, 1, 0, 0},
      {0, 0x3, 0xffffffff, 0, 0},
      {0, 0x3, 0, 0xffffffff, 1},

      // XOR
      {0, 0x4, 0, 1, 1},
      {0, 0x4, 1, 0, 1},
      {0, 0x4, 1112, 1112, 0},
      {0, 0x4, 0xaaaaaaaa, 0xffffffff, 0x55555555},

      // OR
      {0, 0x6, 0x01, 0x02, 0x03},
      {0, 0x6, 0x11111111, 0x44444444, 0x55555555},
      {0, 0x6, 0xaaaaaaaa, 0x55555555, 0xffffffff},

      // AND
      {0, 0x7, 0x01, 0x02, 0x00},
      {0, 0x7, 0x11111111, 0x44444444, 0},
      {0, 0x7, 0xffffffff, 0x12345678, 0x12345678},
      {0, 0x7, 0x12345678, 0xffffffff, 0x12345678},
      {0, 0x7, 0x12345678, 0xf0f0f0f0, 0x10305070},

      // SLL
      {0, 0x1, 0x1, 0x1, 0x02},
      {0, 0x1, 0xabc, 12, 0xabc000},
      {0, 0x1, 0xabc, 24, 0xbc000000},
      {0, 0x1, 0xffffffff, 31, 0x80000000},
      {0, 0x1, 0xffffffff, 32, 0xffffffff},

      // SRL
      {0, 0x5, 0x02, 0x1, 0x1},
      {0, 0x5, 0xabc000, 12, 0xabc},
      {0, 0x5, 0xbc000000, 24, 0xbc},
      {0, 0x5, 0x80000000, 31, 0x1},
      {0, 0x5, 0xffffffff, 32, 0xffffffff},

      // SRA
      {1, 0x5, 0x02, 0x1, 0x1},
      {1, 0x5, 0xabc000, 12, 0xabc},
      {1, 0x5, 0xbc000000, 24, 0xffffffbc},
      {1, 0x5, 0x3c000000, 24, 0x3c},
      {1, 0x5, 0x80000000, 31, 0xffffffff},
      {1, 0x5, 0xffffffff, 31, 0xffffffff},
      {1, 0x5, 0xffffffff, 3, 0xffffffff},
      {1, 0x5, 0xffffffff, 32, 0xffffffff},
  };

  for (auto &tc : test_cases) {
    SCOPED_TRACE(fmt::format("ctl={}:{:x} a={:08x} b={:08x}",
                             uint32_t(tc.ctlhi), uint32_t(tc.ctl), tc.a, tc.b));

    alu.ctl = (tc.ctlhi << 3)| tc.ctl;
    alu.a = tc.a;
    alu.b = tc.b;
    alu.eval();

    EXPECT_EQ(alu.result, tc.expect);
  }

  alu.final();
}
