/**
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYRIGHT in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: Inet4Addr_test.cpp
 * @author: Steven R. Emmerson
 *
 * This file tests class `Inet6Addr`.
 */

#include "Inet6Addr.h"

#include <arpa/inet.h>
#include <cstring>
#include <netinet/in.h>
#include <gtest/gtest.h>

namespace {

// The fixture for testing class Foo.
class Inet6AddrTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  Inet6AddrTest() {
    // You can do set-up work for each test here.
  }

  virtual ~Inet6AddrTest() {
    // You can do clean-up work that doesn't throw exceptions here.
  }

  // If the constructor and destructor are not enough for setting up
  // and cleaning up each test, you can define the following methods:

  virtual void SetUp() {
    // Code here will be called immediately after the constructor (right
    // before each test).
  }

  virtual void TearDown() {
    // Code here will be called immediately after each test (right
    // before the destructor).
  }

  // Objects declared here can be used by all tests in the test case for Foo.
};

static const char* ADDR0 = "::";
static const char* ADDR1 = "2001:db8::ff00:42:8329";
static const char* ADDR2 = "2001:db8::ff00:42:8330";

// Tests default construction
TEST_F(Inet6AddrTest, DefaultConstruction) {
    EXPECT_STREQ(ADDR0, hycast::Inet6Addr().to_string().data());
}

// Tests construction from an IPv6 address string
TEST_F(Inet6AddrTest, IPv6StringConstruction) {
    EXPECT_STREQ(ADDR1, hycast::Inet6Addr(ADDR1).to_string().data());
}

// Tests construction from a struct in6_addr
TEST_F(Inet6AddrTest, in_addr_Construction) {
    struct in6_addr inAddr;
    EXPECT_EQ(1, inet_pton(AF_INET6, ADDR1, &inAddr.s6_addr));
    EXPECT_STREQ(ADDR1, hycast::Inet6Addr(inAddr).to_string().data());
}

// Tests copy construction
TEST_F(Inet6AddrTest, CopyConstruction) {
    hycast::Inet6Addr a1{ADDR1};
    hycast::Inet6Addr a2{a1};
    EXPECT_STREQ(ADDR1, a2.to_string().data());
}

// Tests copy assignment
TEST_F(Inet6AddrTest, CopyAssignment) {
    hycast::Inet6Addr a1{ADDR1};
    hycast::Inet6Addr a2{};
    a2 = a1;
    EXPECT_STREQ(ADDR1, a2.to_string().data());
    hycast::Inet6Addr a3{ADDR2};
    a1 = a3;
    EXPECT_STREQ(ADDR1, a2.to_string().data());
}

// Tests hash()
TEST_F(Inet6AddrTest, hash) {
    hycast::Inet6Addr inAddr1{ADDR1};
    hycast::Inet6Addr inAddr2{ADDR2};
    EXPECT_TRUE(inAddr1.hash() != inAddr2.hash());
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
