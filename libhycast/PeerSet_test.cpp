/**
 * This file tests class `PeerSet`.
 *
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: PeerSet_test.cpp
 * @author: Steven R. Emmerson
 */


#include "PeerSet.h"
#include "PeerSet.h"
#include "ProdInfo.h"

#include <gtest/gtest.h>

namespace {

// The fixture for testing class PeerSet.
class PeerSetTest : public ::testing::Test {
protected:
    // You can remove any or all of the following functions if its body
    // is empty.

    PeerSetTest() {
        // You can do set-up work for each test here.
    }

    virtual ~PeerSetTest() {
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

    // Objects declared here can be used by all tests in the test case for PeerSet.
    hycast::PeerSet peerSet{};
#if 0
    hycast::ProdInfo prodInfo("product", hycast::ProdIndex(1), 100000, 32000);
    class myPeerMgr : public hycast::PeerMgr {

    } peerMgr;
#endif
};

// Tests default construction
TEST_F(PeerSetTest, DefaultConstruction) {
    hycast::PeerSet peerSet{};
}

// Tests inserting a peer
TEST_F(PeerSetTest, PeerInsertion) {
    hycast::Peer peer{};
    peerSet.insert(peer);
}

#if 0
// Tests sending a product notice
TEST_F(PeerSetTest, SendProdNotice) {
    hycast::Peer peer(peerMgr, sock);
    peerSet.insert(peer);
    peerSet.sendNotice(prodInfo);
}
#endif

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
