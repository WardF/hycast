/**
 * This file tests class `Repository`.
 *
 * Copyright 2019 University Corporation for Atmospheric Research. All rights
 * reserved. See file "COPYING" in the top-level source-directory for usage
 * restrictions.
 *
 *       File: Repository_test.cpp
 * Created On: Dec 23, 2019
 *     Author: Steven R. Emmerson
 */
#include "config.h"

#include "error.h"
#include "FileUtil.h"
#include "hycast.h"
#include "Repository.h"

#include <fcntl.h>
#include <gtest/gtest.h>
#include <sys/stat.h>

namespace {

/// The fixture for testing class `Repository`
class RepositoryTest : public ::testing::Test
{
protected:
    const std::string     testDir;
    const std::string     rootDir;
    std::string           prodName;
    const std::string     filePath;
    hycast::ProdIndex     prodIndex;
    char                  memData[1000];
    const hycast::SegSize segSize;
    hycast::ProdSize      prodSize;
    hycast::ProdInfo      prodInfo;
    hycast::SegId         segId;
    hycast::SegInfo       segInfo;
    hycast::MemSeg        memSeg;

    RepositoryTest()
        : testDir("/tmp/Repository_test")
        , rootDir(testDir + "/repo")
        , prodName{"foo/bar/product.dat"}
        , filePath(testDir + "/" + prodName)
        , prodIndex{1}
        , memData{}
        , segSize{sizeof(memData)}
        , prodSize{segSize}
        , prodInfo{prodIndex, prodSize, prodName}
        , segId(prodIndex, 0)
        , segInfo(segId, prodSize, segSize)
        , memSeg{segInfo, memData}
    {
        hycast::rmDirTree(testDir);
        hycast::ensureDir(hycast::dirPath(filePath));
        ::memset(memData, 0xbd, segSize);
    }

    ~RepositoryTest() {
        hycast::rmDirTree(testDir);
    }

public:
    void newProd(const hycast::ProdInfo& actualProdInfo)
    {
        EXPECT_EQ(prodInfo, actualProdInfo);
    }

    void completed(const hycast::ProdInfo& actualProdInfo)
    {
        EXPECT_EQ(prodInfo, actualProdInfo);
    }
};

// Tests construction
TEST_F(RepositoryTest, Construction)
{
    hycast::PubRepo pubRepo(rootDir, segSize);
    hycast::SubRepo subRepo(rootDir, segSize);
}

// Tests saving just product-information
TEST_F(RepositoryTest, SaveProdInfo)
{
    hycast::SubRepo repo(rootDir, segSize);
    ASSERT_FALSE(repo.getProdInfo(prodIndex));
    ASSERT_TRUE(repo.save(prodInfo));
    auto actual = repo.getProdInfo(prodIndex);
    ASSERT_TRUE(actual);
    EXPECT_EQ(prodInfo, actual);
}

// Tests saving product-information and then the data
TEST_F(RepositoryTest, SaveInfoThenData)
{
    hycast::SubRepo repo(rootDir, segSize);

    ASSERT_TRUE(repo.save(prodInfo));
    ASSERT_TRUE(repo.save(memSeg));

    auto prodInfo = repo.getNextProd();
    ASSERT_TRUE(prodInfo);
    EXPECT_EQ(RepositoryTest::prodInfo, prodInfo);

    auto actual = repo.getMemSeg(segId);
    ASSERT_TRUE(actual);
    ASSERT_EQ(memSeg, actual);
}

// Tests saving product-data and then product-information
TEST_F(RepositoryTest, SaveDataThenInfo)
{
    hycast::SubRepo repo(rootDir, segSize);

    ASSERT_TRUE(repo.save(memSeg));
    ASSERT_TRUE(repo.save(prodInfo));

    auto prodInfo = repo.getNextProd();
    ASSERT_TRUE(prodInfo);
    EXPECT_EQ(RepositoryTest::prodInfo, prodInfo);

    auto actual = repo.getMemSeg(memSeg.getSegId());
    ASSERT_TRUE(actual);
    ASSERT_EQ(memSeg, actual);
}

// Tests creating a product and informing a publisher's repository
TEST_F(RepositoryTest, CreatProdForSending)
{
    // Create file
    int fd = ::open(filePath.data(), O_WRONLY|O_CREAT|O_EXCL, 0600);
    ASSERT_NE(-1, fd);
    ASSERT_EQ(segSize, ::write(fd, memData, segSize));
    ASSERT_EQ(0, ::close(fd));

    // Create the publisher's repository and tell it about the file
    hycast::PubRepo repo(rootDir, segSize);
    repo.link(filePath, prodInfo.getProdName());

    // Verify repository access
    try {
        auto prodInfo = repo.getNextProd();
        ASSERT_TRUE(RepositoryTest::prodInfo == prodInfo);
        auto memSeg = repo.getMemSeg(RepositoryTest::segId);
        ASSERT_EQ(RepositoryTest::memSeg, memSeg);
    }
    catch (const std::exception& ex) {
        LOG_ERROR(ex, "Couldn't verify repository access");
        GTEST_FAIL();
    }
}
#if 0
#endif

}  // namespace

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
