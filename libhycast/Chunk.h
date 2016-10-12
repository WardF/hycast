/**
 * This file declares two types of chunks of data: 1) a latent chunk that must
 * be read from an object channel; and 2) a reified chunk with a pointer to its
 * data. The two types are in the same file to support keeping their
 * serialization and de-serialization methods consistent.
 *
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: Chunk.h
 * @author: Steven R. Emmerson
 */

#ifndef CHUNK_H_
#define CHUNK_H_

#include "HycastTypes.h"
#include "ChunkInfo.h"
#include "Channel.h"
#include "Socket.h"

namespace hycast {

class LatentChunk {
    ChunkInfo          info;
    Channel<ChunkInfo> channel;
    ChunkSize          size;
public:
    /**
     * Constructs from an object channel whose current message is a chunk of
     * data and a protocol version. NB: This method reads the current message.
     * @param[in] channel  Object channel
     * @param[in] version  Protocol version
     * @throws std::invalid_argument if the current message is invalid
     */
    LatentChunk(
            Channel<ChunkInfo>& channel,
            const unsigned      version);
    /**
     * Returns information on the chunk.
     * @return information on the chunk
     * @exceptionsafety Strong
     * @threadsafety Safe
     */
    const ChunkInfo& getInfo() const {
        return info;
    }
    /**
     * Returns the size of the chunk of data.
     * @return the size of the chunk of data
     * @exceptionsafety Strong
     * @threadsafety Safe
     */
    ChunkSize getSize() const {
        return size;
    }
    /**
     * Drains the chunk of data into a buffer. The latent data will no longer
     * be available.
     * @param[in] buf  Buffer to drain the chunk of data into
     * @throws std::system_error if an I/O error occurs
     * @exceptionsafety Basic
     * @threadsafety Safe
     */
    void drainData(void* buf);
};

class ActualChunk {
    ChunkInfo   info;
    const void* data;
    ChunkSize   size;
public:
    /**
     * Constructs from information on the chunk and a pointer to its data.
     * @param[in] info  Chunk information
     * @param[in] data  Chunk data
     * @param[in] size  Amount of data in bytes
     */
    ActualChunk(
            const ChunkInfo& info,
            const void*      data,
            const ChunkSize  size)
        : info(info),
          data(data),
          size(size) {}
    /**
     * Returns information on the chunk.
     * @return information on the chunk
     * @exceptionsafety Nothrow
     * @threadsafety Safe
     */
    const ChunkInfo& getInfo() const noexcept {
        return info;
    }
    /**
     * Returns the size of the chunk of data.
     * @return the size of the chunk of data
     * @exceptionsafety Nothrow
     * @threadsafety Safe
     */
    ChunkSize getSize() const noexcept {
        return size;
    }
    /**
     * Returns a pointer to the data.
     * @returns a pointer to the data
     * @exceptionsafety Nothrow
     * @threadsafety Safe
     */
    const void* getData() const noexcept {
        return data;
    }
    /**
     * Serializes this instance to an object channel. NB: This is the only thing
     * that goes out on the channel.
     * @param[in,out] channel  Object channel
     * @param[in]     version  Protocol version
     * @exceptionsafety Basic
     * @threadsafety Compatible but not safe
     */
    void serialize(
            Channel<ChunkInfo>& channel,
            const unsigned      version);
};

} // namespace

#endif /* CHUNK_H_ */