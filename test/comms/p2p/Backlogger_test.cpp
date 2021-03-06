/**
 * This file tests class `Backlogger`.
 *
 * Copyright 2017 University Corporation for Atmospheric Research. All rights
 * reserved. See file "COPYING" in the top-level source-directory for usage
 * restrictions.
 *
 *       File: Backlogger_test.cpp
 * Created On: Sep 21, 2017
 *     Author: Steven R. Emmerson
 */
#include "config.h"

#include "Backlogger.h"
#include "ProdInfo.h"

#include "gtest/gtest.h"

namespace {

/// The fixture for testing class `Backlogger`
class BackloggerTest : public ::testing::Test
{
protected:
    BackloggerTest() {
        hycast::ProdInfo   prodInfo{"Product 0", 0,
                2u*hycast::ChunkId::getCanonSize()};
        startWith = hycast::ChunkId{prodInfo, 1};
    }

    hycast::ChunkId  startWith;
};

// Tests construction
TEST_F(BackloggerTest, DefaultConstruction)
{
    hycast::Backlogger backlogger{startWith};
    EXPECT_EQ(startWith, backlogger.getStart());
}

// Tests that doNotSend() initially returns empty chunk-information
TEST_F(BackloggerTest, InitialChunkInfo)
{
    hycast::Backlogger backlogger{startWith};
    EXPECT_FALSE(static_cast<bool>(backlogger.getEarliest()));
}

// Tests doNotSend function
TEST_F(BackloggerTest, DoNotRequest)
{
    hycast::Backlogger backlogger{startWith};

    const hycast::ProdInfo   prodInfo1{"Product 1", 1, 50};
    const hycast::ChunkId  chunkInfo1{prodInfo1, 0};
    backlogger.doNotNotifyOf(chunkInfo1);
    EXPECT_EQ(chunkInfo1, backlogger.getEarliest());

    const hycast::ProdInfo prodInfo0{"Product 0", 0, 128000};
    const hycast::ChunkId chunkInfo2{prodInfo0, 1};
    backlogger.doNotNotifyOf(chunkInfo2);
    EXPECT_EQ(chunkInfo2, backlogger.getEarliest());

    const hycast::ChunkId chunkInfo3{prodInfo0, 0};
    backlogger.doNotNotifyOf(chunkInfo3);
    EXPECT_EQ(chunkInfo3, backlogger.getEarliest());

    const hycast::ChunkId chunkInfo4{prodInfo0, 2};
    backlogger.doNotNotifyOf(chunkInfo4);
    EXPECT_EQ(chunkInfo3, backlogger.getEarliest());
}

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
