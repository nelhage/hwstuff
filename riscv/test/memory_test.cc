#include <gtest/gtest.h>

#include "Vmemory.h"

#include "svdpi.h"
#include <verilated.h>
#include <verilated_syms.h>

#include <fmt/core.h>

#include <fstream>
#include <iostream>
#include <memory>
#include <vector>

using std::ios;
using std::make_unique;

TEST(RISCVTest, MemoryTest) {
  auto contextp = std::make_unique<VerilatedContext>();

  Vmemory memory;

  auto scope = svGetScopeFromName("TOP.memory");
  ASSERT_NE(scope, nullptr);

  std::vector<uint32_t> mem_fill;
  mem_fill.resize(1 << 16);
  for (auto i = 0; i < mem_fill.size(); i++) {
    mem_fill[i] =
        static_cast<uint32_t>((uint64_t(0xbad4d0eecafecafe) * i) >> 32);
  }
  {
    std::ofstream out("testmem.dat", ios::out | ios::trunc);
    for (auto it = mem_fill.begin(); it != mem_fill.end(); ++it) {
      out << fmt::format("{:08x}\n", *it);
    }
  }

  svSetScope(scope);
  memory.initialize_memory("testmem.dat");

  memory.clk = 0;
  memory.eval();

  // combinatorial read
  memory.we = 0;
  for (auto i = 0; i < mem_fill.size(); i += 129) {
    memory.ra = i << 2;
    memory.eval();
    EXPECT_EQ(memory.rd, mem_fill[i]) << fmt::format("addr={}", i);
  }

  // writes
  memory.wa = 512;
  memory.wd = 0xabcd4321;
  memory.eval();
  memory.we = 1;
  memory.clk = 1;
  memory.eval();

  memory.ra = 512;
  memory.eval();
  EXPECT_EQ(memory.rd, 0xabcd4321);

  memory.clk = 0;
  memory.wa = 512;
  memory.wd = 0xd00d;
  memory.we = 0;
  memory.eval();
  memory.clk = 1;
  memory.eval();

  EXPECT_EQ(memory.rd, 0xabcd4321);

  memory.ra = (mem_fill.size()) << 2;
  memory.eval();
  EXPECT_EQ(memory.rd, 0);

  memory.ra = (mem_fill.size() - 1) << 2;
  memory.eval();
  EXPECT_EQ(memory.rd, mem_fill.back());

  memory.final();
}
