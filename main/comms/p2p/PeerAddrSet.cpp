/**
 * This file implements an entry for an available chunk-of-data that's been
 * requested but not yet received.
 *
 * Copyright 2017 University Corporation for Atmospheric Research. All Rights
 * reserved. See file "Copying" in the top-level source-directory for usage
 * restrictions.
 *
 *        File: PeerAddrSet.cpp
 *  Created on: Dec 7, 2017
 *      Author: Steven R. Emmerson
 */

#include "PeerAddrSet.h"

#include <chrono>
#include <set>

namespace hycast {

class PeerAddrSet::Impl final
{
    std::set<InetSockAddr> peerAddrs;

public:
    Impl()
        : peerAddrs{}
    {}

    size_t size() const
    {
        return peerAddrs.size();
    }

    void add(const InetSockAddr& peerAddr) {
        peerAddrs.insert(peerAddr);
    }

    void remove(const InetSockAddr& peerAddr) {
        peerAddrs.erase(peerAddr);
    }

    bool getRandom(
            InetSockAddr&               peerAddr,
            std::default_random_engine& generator) const
    {
        auto distrib = std::uniform_int_distribution<int>{0,
                static_cast<int>(peerAddrs.size())-1};
        auto targetIndex = distrib(generator);
        auto index = 0;
        for (const InetSockAddr& addr : peerAddrs) {
            if (index++ == targetIndex) {
                peerAddr = addr;
                return true;
            }
        }
        return false;
    }
};

PeerAddrSet::PeerAddrSet()
    : pImpl{new Impl()}
{}

size_t PeerAddrSet::size() const
{
    return pImpl->size();
}

void PeerAddrSet::add(const InetSockAddr& peerAddr)
{
    pImpl->add(peerAddr);
}

void PeerAddrSet::remove(const InetSockAddr& peerAddr)
{
    pImpl->remove(peerAddr);
}

bool PeerAddrSet::getRandom(
        InetSockAddr&               peerAddr,
        std::default_random_engine& generator) const
{
    return pImpl->getRandom(peerAddr, generator);
}

} // namespace
