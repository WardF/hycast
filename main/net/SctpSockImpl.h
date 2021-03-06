/**
 * This file declares the implementation of an SCTP socket.
 *
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: SctpSockImpl.h
 * @author: Steven R. Emmerson
 */

#ifndef MAIN_NET_SCTPSOCKIMPL_H_
#define MAIN_NET_SCTPSOCKIMPL_H_

#include "InetSockAddr.h"
#include "SctpSock.h"

#include <atomic>
#include <cstdint>
#include <errno.h>
#include <mutex>
/*
 * <netinet/in.h> must be included before <netinet/sctp.h> on Fedora 19 to avoid
 * a macro/enum name clash on IPPROTO_SCTP.
 */
#include <netinet/in.h>
#include <netinet/sctp.h>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

namespace hycast {

class SctpSock::Impl
{
protected:
    std::atomic_int sock;

private:
    unsigned     streamId;
    uint32_t     size;
    bool         haveCurrMsg;
    unsigned     numStreams;
    std::mutex   readMutex;
    std::mutex   writeMutex;
    InetSockAddr remoteAddr;

    /**
     * Checks the return status of an I/O function.
     * @param[in] funcName  Name of the I/O function
     * @param[in] expected  The expected return status or 0, which disables the
     *                      comparison with `actual`
     * @param[in] actual    The actual return status
     * @throws std::system_error if `actual < 0 || (expected && actual !=
     *                           expected)`
     * @exceptionsafety Strong
     */
    void checkIoStatus(
            const char*   funcName,
            const size_t  expected,
            const ssize_t actual) const;

    /**
     * Gets information on the next SCTP message. The message is left in the
     * socket's input buffer.
     * @throws std::system_error if an I/O error occurs
     * @exceptionsafety Basic
     * @threadsafety Safe
     */
    void getNextMsgInfo();

    /**
     * Computes the total number of bytes in a scatter/gather IO operation.
     * @param[in] iovec   Scatter/gather IO vector
     * @param[in] iovcnt  Number of elements in `iovec`
     */
    static size_t iovLen(
            const struct iovec* iovec,
            const int           iovcnt) noexcept;

    /**
     * Initializes an SCTP send/receive information structure.
     * @param[out] sinfo     SCTP send/receive information structure
     * @param[in]  streamId  SCTP stream number
     * @param[in]  size      Size of the message in bytes
     * @exceptionsafety Nothrow
     */
    static void sndrcvinfoInit(
            struct sctp_sndrcvinfo& sinfo,
            const unsigned          streamId,
            const size_t            size) noexcept;

    /**
     * Ensures that the current message exists.
     * @throws std::system_error if an I/O error occurs
     * @exceptionsafety Basic
     * @threadsafety Safe
     */
    void ensureMsg();

public:
    /**
     * Constructs from nothing.
     * @throws std::bad_alloc if required memory can't be allocated
     */
    Impl();

    /**
     * Constructs from a socket and the number of SCTP streams. If the socket
     * isn't connected to a remote endpoint, then getRemoteAddr() will return
     * a default-constructed `InetSockAddr`.
     * @param[in] sd                  Socket descriptor
     * @param[in] numStreams          Number of SCTP streams
     * @throws std::invalid_argument  `sock < 0 || numStreams > UINT16_MAX`
     * @throws std::system_error      Socket couldn't be configured
     * @see getRemoteAddr()
     */
    Impl(
            const int      sd,
            const unsigned numStreams);

    /**
     * Prevents copy construction.
     */
    Impl(const Impl& socket) =delete;

    /**
     * Destroys an instance. Closes the underlying BSD socket.
     * @exceptionsafety Nothrow
     */
    ~Impl() noexcept;

    /**
     * Prevents copy assignment.
     */
    Impl& operator=(const Impl& rhs) =delete;

    /**
     * Returns the number of SCTP streams.
     * @return the number of SCTP streams
     */
    unsigned getNumStreams();

    /**
     * Returns the Internet socket address of the remote end.
     * @return Internet socket address of the remote end
     */
    const InetSockAddr& getRemoteAddr();

    /**
     * Indicates if this instance equals another.
     * @param[in] that  Other instance
     * @retval `true`   This instance equals the other
     * @retval `false`  This instance doesn't equal the other
     * @exceptionsafety Nothrow
     */
    bool operator==(const Impl& that) const noexcept;

    /**
     * Returns a string representation of this instance's socket.
     * @return String representation of this instance's socket
     * @throws std::bad_alloc if required memory can't be allocated
     * @exceptionsafety Strong
     * @threadsafety    Safe
     */
    std::string to_string() const;

    /**
     * Sends a message.
     * @param[in] streamId  SCTP stream number
     * @param[in] msg       Message to be sent
     * @param[in] len       Size of message in bytes
     * @throws std::system_error if an I/O error occurs
     * @exceptionsafety Basic
     * @threadsafety Safe
     */
    void send(
            const unsigned streamId,
            const void*    msg,
            const size_t   len);

    /**
     * Sends a message.
     * @param[in] streamId  SCTP stream number
     * @param[in] iovec     Vector comprising message to send
     * @param[in] iovcnt    Number of elements in `iovec`
     * @throws std::system_error if an I/O error occurs
     * @exceptionsafety Basic
     * @threadsafety Safe
     */
    void sendv(
            const unsigned      streamId,
            const struct iovec* iovec,
            const int           iovcnt);

    /**
     * Returns the size, in bytes, of the current SCTP message. Waits for the
     * next message if necessary.
     * @return Size of current message in bytes or 0 if the remove peer closed
     *         the socket.
     */
    uint32_t getSize();

    /**
     * Returns the SCTP stream number of the current SCTP message. Waits for the
     * next message if necessary.
     * @return SCTP stream number of current message.
     * @throws std::system_error if an I/O error occurs
     */
    unsigned getStreamId();

    /**
     * Receives a message.
     * @param[out] msg     Receive buffer
     * @param[in]  len     Size of receive buffer in bytes
     * @param[in]  flags   Type of message reception. Logical OR of zero or
     *                     more of
     *                       - `MSG_OOB`  Requests out-of-band data
     *                       - `MSG_PEEK` Peeks at the incoming message
     * @throws std::system_error on I/O failure or if number of bytes read
     *     doesn't equal `len`
     * @exceptionsafety Basic
     * @threadsafety Safe
     */
    void recv(
            void*        msg,
            const size_t len,
            const int    flags = 0);

    /**
     * Receives a message.
     * @param[in] iovec     Vector comprising message receive buffers
     * @param[in] iovcnt    Number of elements in `iovec`
     * @param[in] flags     Type of message reception. Logical OR of zero or
     *                      more of
     *                      - `MSG_OOB`  Requests out-of-band data
     *                      - `MSG_PEEK` Peeks at the incoming message
     * @return              Number of bytes actually read
     * @throws std::system_error if an I/O error occurs
     * @exceptionsafety Basic guarantee
     * @threadsafety Safe
     */
    size_t recvv(
            const struct iovec* iovec,
            const int           iovcnt,
            const int           flags = 0);

    /**
     * Indicates if this instance has a current message.
     * @retval true   Yes
     * @retval false  No
     */
    bool hasMessage();

    /**
     * Discards the current message.
     * @exceptionsafety Basic guarantee
     * @threadsafety    Thread-compatible but not thread-safe
     */
    void discard();

    /**
     * Closes the underlying BSD socket.
     * @exceptionsafety Nothrow
     * @threadsafety    Compatible but not safe
     */
    void close();
};

} // namespace

#endif /* MAIN_NET_SCTPSOCKIMPL_H_ */
