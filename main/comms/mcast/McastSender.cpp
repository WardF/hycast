/**
 * This file implements a multicast sender of data-products.
 *
 * Copyright 2017 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: McastSender.cpp
 * @author: Steven R. Emmerson
 */
#include "config.h"

#include "error.h"
#include "Codec.h"
#include "McastSender.h"
#include "UdpSock.h"

namespace hycast {

class McastSender::Impl final
{
    class Enc final : public Encoder
    {
        OutUdpSock sock;

    protected:
        virtual void write(
                struct iovec* iov,
                const int     iovcnt)
        {
            sock.send(iov, iovcnt);
        }

    public:
        Enc(const InetSockAddr& mcastAddr)
            : Encoder(UdpSock::maxPayload)
            , sock(mcastAddr)
        {}
    };

    Enc            encoder;
    const unsigned version;

    /**
     * Multicasts a product-information datagram.
     * @param[in] prodInfo  Information on the data-product
     */
    void send(const ProdInfo prodInfo)
    {
        try {
            LOG_DEBUG("Multicasting product-info: prodIndex=%s",
                    std::to_string(prodInfo.getIndex()).c_str());
            encoder.encode(prodInfoMsgId);
            prodInfo.serialize(encoder, version);
            encoder.flush();
        }
        catch (const std::exception& ex) {
            std::throw_with_nested(RUNTIME_ERROR(
                    "Couldn't multicast product-info " +
                    std::to_string(prodInfo)));
        }
    }

    void writeInfoChunk(
            const std::string& name,
            const ProdSize     prodSize,
            UdpSndrSock&       sock) const
    {
        const uint16_t isInfo = 1;
        struct iovec   iov[5];

        isInfo = sock.hton(isInfo);
        iov[0].iov_base = &isInfo;
        iov[0].iov_len = sizeof(isInfo);

        uint16_t nameSize = name.length();
        nameSize = sock.hton(nameSize);
        iov[1].iov_base = &nameSize;
        iov[1].iov_len = sizeof(nameSize);

        auto prodIndex = sock.hton(chunkId.getProdIndex());
        iov[2].iov_base = &prodIndex;
        iov[2].iov_len = sizeof(prodIndex);

        prodSize = sock.hton(prodSize);
        iov[3].iov_base = &prodSize;
        iov[3].iov_len = sizeof(prodSize);

        iov[4].iov_base = const_cast<void*>(name.data()); // Safe cast
        iov[4].iov_len = size;

        sock.write(iov, 5);
    }

    void writeDataChunk(
            SegSize      dataSize,
            SegIndex     segIndex,
            SegIndex     numSegs,
            UdpSndrSock& sock) const
    {
        const uint16_t isInfo = 0;
        struct iovec   iov[5];

        iov[0].iov_base = &isInfo;
        iov[0].iov_len = sizeof(isInfo);

        dataSize = sock.hton(dataSize);
        iov[1].iov_base = &dataSize;
        iov[1].iov_len = sizeof(dataSize);

        auto prodIndex = sock.hton(chunkId.getProdIndex());
        iov[2].iov_base = &prodIndex;
        iov[2].iov_len = sizeof(prodIndex);

        segIndex = sock.hton(segIndex);
        iov[3].iov_base = &segIndex;
        iov[3].iov_len = sizeof(segIndex);

        numSegs = sock.hton(numSegs);
        iov[4].iov_base = &numSegs;
        iov[4].iov_len = sizeof(numSegs);

        iov[5].iov_base = const_cast<void*>(data); // Safe cast
        iov[5].iov_len = size;

        sock.write(iov, 6);
    }

public:
    /**
     * Constructs.
     * @param[in] mcastAddr  Socket address of the multicast group
     * @param[in] version    Protocol version
     * @throws std::system_error  `socket()` failure
     */
    Impl(   const InetSockAddr& mcastAddr,
            const unsigned      version)
        : encoder(mcastAddr)
        , version{version}
    {}

    /**
     * Multicasts a data-product.
     * @param[in] prod  Data-product to be multicasted
     * @exceptionsafety Basic guarantee
     * @threadsafety    Safe
     */
    void send(Product& prod)
    {
        try {
            // Keep consistent with McastReceiver::operator()()
            const ProdInfo prodInfo = prod.getInfo();
            send(prodInfo);

            const auto prodIndex = prodInfo.getIndex();
            const auto numChunks = prodInfo.getNumChunks();
            for (ChunkIndex chunkIndex = 0; chunkIndex < numChunks; ++chunkIndex) {
                encoder.encode(chunkMsgId);
                auto chunk = prod.getChunk(chunkIndex);
                if (!chunk)
                    throw RUNTIME_ERROR("Chunk " + std::to_string(chunkIndex) +
                            " doesn't exist");
                chunk.serialize(encoder, version);
                encoder.flush();
            }
        }
        catch (const std::exception& ex) {
            std::throw_with_nested(RUNTIME_ERROR("Couldn't multicast product " +
                    std::to_string(prod.getInfo())));
        }
    }
};

void McastSender::send(Product& prod)
{
    pImpl->send(prod);
}

McastSender::McastSender(
        const InetSockAddr& mcastAddr,
        const unsigned      version)
    : pImpl{new Impl(mcastAddr, version)}
{}

} // namespace
