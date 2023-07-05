#include <gtest/gtest.h>

#include "Vmemory.h"
#include "Vmemory_memory.h"

#include "svdpi.h"
#include <verilated.h>
#include <verilated_syms.h>

#include <fmt/core.h>

#include <vector>

using std::ios;

TEST(MemoryTest, Basic) {
  auto contextp = std::make_unique<VerilatedContext>();

  Vmemory memory;

  const size_t nmem = sizeof(memory.memory->RAM.m_storage)/sizeof(memory.memory->RAM.m_storage[0]);
  std::vector<uint32_t> mem_fill(nmem);
  for (auto i = 0; i < mem_fill.size(); i++) {
    mem_fill[i] =
        static_cast<uint32_t>((uint64_t(0xbad4d0eecafecafe) * i) >> 32);
    memory.memory->RAM.m_storage[i] = mem_fill[i];
  }

  memory.clk = 0;
  memory.eval();

  // combinatorial read
  memory.we = 0;
  for (auto i = 0; i < mem_fill.size(); i += 129) {
    memory.ra1 = i << 2;
    memory.ra2 = 0;
    memory.eval();
    EXPECT_EQ(memory.rd1, mem_fill[i]) << fmt::format("addr={}", i);
    EXPECT_EQ(memory.rd2, mem_fill[0]);

    memory.ra2 = i << 2;
    memory.ra1 = 0;
    memory.eval();
    EXPECT_EQ(memory.rd2, mem_fill[i]) << fmt::format("addr={}", i);
    EXPECT_EQ(memory.rd1, mem_fill[0]);
  }

  // writes
  memory.wa = 512;
  memory.wd = 0xabcd4321;
  memory.eval();
  memory.we = 1;
  memory.clk = 1;
  memory.eval();

  memory.ra1 = 512;
  memory.eval();
  EXPECT_EQ(memory.rd1, 0xabcd4321);

  memory.clk = 0;
  memory.wa = 512;
  memory.wd = 0xd00d;
  memory.we = 0;
  memory.eval();
  memory.clk = 1;
  memory.eval();

  EXPECT_EQ(memory.rd1, 0xabcd4321);

  memory.ra1 = (mem_fill.size()) << 2;
  memory.eval();
  EXPECT_EQ(memory.rd1, 0);

  memory.ra1 = (mem_fill.size() - 1) << 2;
  memory.eval();
  EXPECT_EQ(memory.rd1, mem_fill.back());

  memory.final();
}
