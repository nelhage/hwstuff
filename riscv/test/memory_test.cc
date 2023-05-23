#include <gtest/gtest.h>

#include "Vmemory.h"
#include <memory>
#include <verilated.h>

using std::make_unique;

TEST(RISCVTest, MemoryTest) {
  auto contextp = std::make_unique<VerilatedContext>();

  Vmemory memory;

  memory.clk = 0;
  memory.eval();

  memory.we = 1;
  memory.wa = 1;
  memory.wd = 0xdeadbeef;
  memory.clk = 1;
  memory.eval();

  memory.clk = 0;
  memory.eval();

  memory.we = 0;
  memory.ra = 1;
  memory.clk = 1;
  memory.eval();

  EXPECT_EQ(memory.rd, 0xdeadbeef);

  memory.final();
}
