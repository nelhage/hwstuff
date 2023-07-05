#include "Vfetchstore.h"
#include <verilated.h>

#include <gtest/gtest.h>

#include <fmt/core.h>

#include <memory>
#include <stdint.h>

using std::make_unique;

const uint32_t MEMW_BYTE = 0b00;
const uint32_t MEMW_HALF = 0b01;
const uint32_t MEMW_WORD = 0b10;

TEST(FetchStore, Read) {
  auto contextp = make_unique<VerilatedContext>();

  Vfetchstore fetch;

  fetch.corew = 0;

  // byte-width, non-sext
  fetch.coresext = 0;
  fetch.corewidth = MEMW_BYTE;
  fetch.busrd = 0xabcdf00d;

  fetch.coreaddr = 120;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 120);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0x0d);

  fetch.coreaddr = 121;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 120);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xf0);

  fetch.coreaddr = 122;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 120);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xcd);

  fetch.coreaddr = 123;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 120);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xab);

  // byte-width, sext
  fetch.coresext = 1;
  fetch.busrd = 0xfa7b81cc;

  fetch.coreaddr = 9000;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 9000);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xffffffcc);

  fetch.coreaddr = 9001;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 9000);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xffffff81);

  fetch.coreaddr = 8566;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 8564);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0x7b);

  fetch.coreaddr = 8567;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 8564);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xfffffffa);

  // half-width, non-sext
  fetch.coresext = 0;
  fetch.busrd = 0xabcd5999;
  fetch.corewidth = MEMW_HALF;

  fetch.coreaddr = 0;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 0);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0x5999);

  fetch.coreaddr = 2;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 0);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xabcd);

  // half-width, sext
  fetch.coresext = 1;
  fetch.coreaddr = 12;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 12);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0x5999);

  fetch.coreaddr = 0x1002;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 0x1000);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xffffabcd);

  // full-width
  fetch.corewidth = MEMW_WORD;
  fetch.coreaddr = 640;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 640);
  EXPECT_EQ(fetch.buswe, 0);
  EXPECT_EQ(fetch.corerd, 0xabcd5999);
}


TEST(FetchStore, Write) {
  auto contextp = make_unique<VerilatedContext>();

  Vfetchstore fetch;

  fetch.corew = 1;

  // byte-width
  fetch.corewidth = MEMW_BYTE;
  fetch.corewd = 0x2b;

  fetch.busrd = 0x012345678;

  fetch.coreaddr = 100;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 100);
  EXPECT_EQ(fetch.buswa, 100);
  EXPECT_EQ(fetch.buswe, 1);
  EXPECT_EQ(fetch.buswd, 0x01234562b);

  fetch.coreaddr = 101;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 100);
  EXPECT_EQ(fetch.buswa, 100);
  EXPECT_EQ(fetch.buswe, 1);
  EXPECT_EQ(fetch.buswd, 0x012342b78);

  fetch.coreaddr = 86;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 84);
  EXPECT_EQ(fetch.buswa, 84);
  EXPECT_EQ(fetch.buswe, 1);
  EXPECT_EQ(fetch.buswd, 0x0122b5678);

  fetch.coreaddr = 87;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 84);
  EXPECT_EQ(fetch.buswa, 84);
  EXPECT_EQ(fetch.buswe, 1);
  EXPECT_EQ(fetch.buswd, 0x2b345678);

  // half-width
  fetch.corewidth = MEMW_HALF;
  fetch.corewd = 0x92d1;

  fetch.busrd = 0xffffffff;

  fetch.coreaddr = 92;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 92);
  EXPECT_EQ(fetch.buswa, 92);
  EXPECT_EQ(fetch.buswe, 1);
  EXPECT_EQ(fetch.buswd, 0xffff92d1);

  fetch.coreaddr = 94;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 92);
  EXPECT_EQ(fetch.buswa, 92);
  EXPECT_EQ(fetch.buswe, 1);
  EXPECT_EQ(fetch.buswd, 0x92d1ffff);

  // full-width
  fetch.corewidth = MEMW_WORD;
  fetch.corewd = 0xa11ba115;

  fetch.busrd = 0xffffffff;

  fetch.coreaddr = 100;
  fetch.eval();
  EXPECT_EQ(fetch.busra, 100);
  EXPECT_EQ(fetch.buswa, 100);
  EXPECT_EQ(fetch.buswe, 1);
  EXPECT_EQ(fetch.buswd, 0xa11ba115);
}
