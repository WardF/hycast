/**
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: SockAddrInet6_test.cpp
 * @author: Steven R. Emmerson
 *
 * This file tests class `InetSockAddr`.
 */

#include "InetSockAddr.h"

#include <arpa/inet.h>
#include <gtest/gtest.h>
#include <netinet/in.h>
#include <stdexcept>
#include <string>
#include <string.h>
#include <sys/socket.h>

namespace {

// The fixture for testing class InetSockAddr.
class InetSockAddrTest : public ::testing::Test {
 protected:
  // You can remove any or all of the following functions if its body
  // is empty.

  InetSockAddrTest()
     : ipv4SockAddr("128.117.140.56", 388),
       ipv6SockAddr("2001:db8::ff00:42:8329", 388)
  {
  }

  virtual ~InetSockAddrTest() {
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

  // Objects declared here can be used by all tests in the test case for
  // SockAddrInet.
  hycast::InetSockAddr ipv4SockAddr;
  hycast::InetSockAddr ipv6SockAddr;
};

// Tests default construction
TEST_F(InetSockAddrTest, DefaultConstruction) {
    hycast::InetSockAddr sockaddr{};
    EXPECT_FALSE(sockaddr);
}

// Tests construction from IP address string and port number
TEST_F(InetSockAddrTest, StringIpAndPortConstruction) {
    hycast::InetSockAddr sockaddr1{"128.117.140.56", 388};
    EXPECT_STREQ("128.117.140.56:388", sockaddr1.to_string().data());
    hycast::InetSockAddr sockaddr2{"2001:db8::ff00:42:8329", 388};
    EXPECT_STREQ("[2001:db8::ff00:42:8329]:388", sockaddr2.to_string().data());
}

// Tests construction from IPv4 socket address
TEST_F(InetSockAddrTest, Ipv4SockAddrConstruction) {
    struct sockaddr_in ipv4SockAddr;
    (void)memset(&ipv4SockAddr, 0, sizeof(ipv4SockAddr));
    ipv4SockAddr.sin_family = AF_INET;
    ipv4SockAddr.sin_port = htons(388);
    EXPECT_EQ(1, inet_pton(AF_INET, "128.117.140.56", &ipv4SockAddr.sin_addr));
    hycast::InetSockAddr sockaddr{ipv4SockAddr};
    EXPECT_STREQ("128.117.140.56:388", sockaddr.to_string().data());
}

// Tests construction from IPv6 address and port number
TEST_F(InetSockAddrTest, Ipv6AddrAndPortConstruction) {
    struct in6_addr ipv6Addr;
    EXPECT_EQ(1, inet_pton(AF_INET6, "2001:db8::ff00:42:8329", &ipv6Addr));
    hycast::InetSockAddr inetSockAddr{ipv6Addr, 388};
    EXPECT_STREQ("[2001:db8::ff00:42:8329]:388", inetSockAddr.to_string().data());
}

// Tests construction from Internet address and port number
TEST_F(InetSockAddrTest, InetAddrAndPortConstruction) {
    const hycast::InetAddr     serverInetAddr{"192.168.132.128"};
    hycast::InetSockAddr       serverAddr{serverInetAddr, 38800};
    EXPECT_STREQ("192.168.132.128:38800", serverAddr.to_string().data());
}

// Tests copy construction
TEST_F(InetSockAddrTest, CopyConstruction) {
    hycast::InetSockAddr sockaddr1{"128.117.140.56", 388};
    hycast::InetSockAddr sockaddr2{sockaddr1};
    EXPECT_STREQ("128.117.140.56:388", sockaddr2.to_string().data());
}

// Tests copy assignment
TEST_F(InetSockAddrTest, CopyAssignment) {
    hycast::InetSockAddr sockaddr1{"128.117.140.56", 388};
    EXPECT_STREQ("128.117.140.56:388", sockaddr1.to_string().data());
    hycast::InetSockAddr sockaddr2{};
    sockaddr2 = sockaddr1;
    EXPECT_STREQ("128.117.140.56:388", sockaddr2.to_string().data());
    hycast::InetSockAddr sockaddr3{"128.117.140.57", 388};
    sockaddr2 = sockaddr3;
    EXPECT_STREQ("128.117.140.56:388", sockaddr1.to_string().data());
}

// Tests equality
TEST_F(InetSockAddrTest, Equality) {
    hycast::InetSockAddr sockaddr1{"128.117.140.56", 388};
    EXPECT_EQ(sockaddr1, sockaddr1);
    hycast::InetSockAddr sockaddr2{"128.117.140.56", 389};
    EXPECT_FALSE(sockaddr1 == sockaddr2);
    hycast::InetSockAddr sockaddr3{"128.117.140.57", 388};
    EXPECT_FALSE(sockaddr1 == sockaddr3);
    hycast::InetSockAddr sockaddr4{"2001:db8::ff00:42:8329", 388};
    EXPECT_FALSE(sockaddr1 == sockaddr4);
    hycast::InetSockAddr sockaddr5{"2001:db8::ff00:42:8328", 388};
    EXPECT_FALSE(sockaddr4 == sockaddr5);
    hycast::InetSockAddr sockaddr6{"2001:db8::ff00:42:8329", 389};
    EXPECT_FALSE(sockaddr4 == sockaddr6);
}

// Tests comparison
TEST_F(InetSockAddrTest, Comparison) {
    hycast::InetSockAddr sockaddr1{"128.117.140.56", 388};
    EXPECT_FALSE(sockaddr1 < sockaddr1);
    hycast::InetSockAddr sockaddr2{"128.117.140.56", 389};
    EXPECT_TRUE(sockaddr1 < sockaddr2);
    EXPECT_FALSE(sockaddr2 < sockaddr1);
    hycast::InetSockAddr sockaddr3{"128.117.140.57", 388};
    EXPECT_TRUE(sockaddr1 < sockaddr3);
    EXPECT_FALSE(sockaddr3 < sockaddr1);
    hycast::InetSockAddr sockaddr4{"2001:db8::ff00:42:8329", 388};
    EXPECT_TRUE(sockaddr1 < sockaddr4);
    EXPECT_FALSE(sockaddr4 < sockaddr1);
    hycast::InetSockAddr sockaddr5{"2001:db8::ff00:42:8330", 388};
    EXPECT_TRUE(sockaddr4 < sockaddr5);
    EXPECT_FALSE(sockaddr5 < sockaddr4);
    hycast::InetSockAddr sockaddr6{"2001:db8::ff00:42:8329", 389};
    EXPECT_TRUE(sockaddr4 < sockaddr6);
    EXPECT_FALSE(sockaddr6 < sockaddr4);
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
