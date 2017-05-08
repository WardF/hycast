/**
 * This file tests class hycast::Receiving.
 *
 * Copyright 2017 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: Receiving_test.cpp
 * @author: Steven R. Emmerson
 */

#include "McastSender.h"
#include "P2pMgr.h"
#include "PeerSet.h"
#include "Processing.h"
#include "ProdStore.h"
#include "Receiving.h"
#include "Shipping.h"
#include "YamlPeerSource.h"

#include <gtest/gtest.h>
#include <iostream>
#include <climits>

namespace {

// The fixture for testing class Receiving.
class ReceivingTest : public ::testing::Test, public hycast::Processing
{
protected:
	void process(hycast::Product prod)
	{
		EXPECT_EQ(this->prod, prod);
	}

	ReceivingTest()
	{
		// gcc 4.8 doesn't support non-trivial designated initializers
		p2pInfo.peerCount = maxPeers;
		p2pInfo.peerSource = &peerSource;
		p2pInfo.serverSockAddr = serverAddr;
		p2pInfo.stasisDuration = stasisDuration;

		// gcc 4.8 doesn't support non-trivial designated initializers
		srcMcastInfo.mcastAddr = mcastAddr;
		srcMcastInfo.srcAddr = hycast::InetAddr(serverInetAddr);

		unsigned char data[128000];
		for (size_t i = 0; i < sizeof(data); ++i)
			data[i] = i % UCHAR_MAX;

		prod = hycast::Product{"product", prodIndex, data, sizeof(data)};
	}

	hycast::PeerSet            peerSet{[](hycast::Peer&){}};
    hycast::ProdStore          prodStore{};
    const in_port_t            port{38800};
    const hycast::InetSockAddr mcastAddr{"232.0.0.0", port};
    const std::string          serverInetAddr{"192.168.132.128"};
    hycast::InetSockAddr       serverAddr{serverInetAddr, port};
    const unsigned             protoVers{0};
    hycast::McastSender        mcastSender{mcastAddr, protoVers};
    hycast::YamlPeerSource     peerSource{"[{inetAddr: " + serverInetAddr +
    	    ", port: " + std::to_string(port) + "}]"};
    hycast::ProdIndex          prodIndex{0};
    const unsigned             maxPeers = 1;
    const unsigned             stasisDuration = 2;
    // gcc 4.8 doesn't support non-trivial designated initializers
    hycast::P2pInfo            p2pInfo;
    // gcc 4.8 doesn't support non-trivial designated initializers
    hycast::SrcMcastInfo       srcMcastInfo;
    const unsigned             version = 0;
    hycast::Product            prod{};
};

// Tests construction
TEST_F(ReceivingTest, Construction) {
    hycast::Receiving{srcMcastInfo, p2pInfo, *this, version};
}

// Tests shipping and receiving a product
TEST_F(ReceivingTest, ShippingAndReceiving) {
	// Create shipper
    hycast::Shipping shipping{prodStore, mcastAddr, version, peerSet,
    	serverAddr};

    ::sleep(1);

    // Create receiver
    hycast::Receiving receiving{srcMcastInfo, p2pInfo, *this, version};

    ::sleep(1);

    // Ship product
    shipping.ship(prod);

    ::sleep(1);
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
