/**
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: SocketImpl.cpp
 * @author: Steven R. Emmerson
 *
 * This file defines a RAII object for a socket.
 */

#include "SocketImpl.h"

#include <arpa/inet.h>
#include <cstdint>
#include <errno.h>
#include <string>
#include <sys/socket.h>
#include <system_error>
#include <unistd.h>

namespace hycast {

SocketImpl::SocketImpl()
    : sock(-1),
      streamId(0),
      size(0),
      needMsg(true),
      numStreams(0),
      readMutex(),
      writeMutex()
{
}

SocketImpl::SocketImpl(
        const int      sd,
        const unsigned numStreams)
    : sock(sd),
      streamId(0),
      size(0),
      needMsg(true),
      numStreams(numStreams),
      readMutex(),
      writeMutex()
{
    if (sock < 0)
        throw std::invalid_argument("Invalid socket: " + std::to_string(sock));
    if (numStreams > UINT16_MAX)
        throw std::invalid_argument("Invalid number of streams: " +
                std::to_string(numStreams));
    struct sctp_event_subscribe events = {0};
    events.sctp_data_io_event = 1;
    int status = setsockopt(sock, IPPROTO_SCTP, SCTP_EVENTS, &events,
            sizeof(events));
    if (status)
        throw std::system_error(errno, std::system_category(),
                "setsockopt() failure: Couldn't subscribe to SCTP data I/O "
                "events: sock=" + std::to_string(sock));
    struct sctp_initmsg sinit = {0};
    sinit.sinit_max_instreams = sinit.sinit_num_ostreams = numStreams;
    status = setsockopt(sock, IPPROTO_SCTP, SCTP_INITMSG, &sinit,
            sizeof(sinit));
    if (status)
        throw std::system_error(errno, std::system_category(),
                "setsockopt() failure: Couldn't configure number of SCTP "
                "streams: sock=" + std::to_string(sock) + ", numStreams=" +
                std::to_string(numStreams));
}

SocketImpl::~SocketImpl() noexcept
{
    (void)close(sock);
}

void SocketImpl::sndrcvinfoInit(
        struct sctp_sndrcvinfo& sinfo,
        const unsigned          streamId,
        const size_t            size) noexcept
{
    sinfo.sinfo_stream = streamId;
    sinfo.sinfo_flags = SCTP_UNORDERED;
    sinfo.sinfo_ppid = htonl(size);
    sinfo.sinfo_timetolive = 30000; // in ms
}

void SocketImpl::checkIoStatus(
        const char* const funcName,
        const size_t      expected,
        const ssize_t     actual) const
{
    if (actual < 0)
        throw std::system_error(errno, std::system_category(),
                std::string(funcName) + " failure: sock=" + std::to_string(sock)
                + ", expected=" + std::to_string(expected));
    if ((size_t)actual != expected)
        throw std::system_error(EIO, std::system_category(),
                std::string(funcName) + " failure: sock=" + std::to_string(sock)
                + ", expected=" + std::to_string(expected) + ", actual=" +
                std::to_string(actual));
}

size_t SocketImpl::iovLen(
        const struct iovec* iovec,
        const int           iovcnt) noexcept
{
    size_t len = 0;
    for (int i = 0; i < iovcnt; i++)
        len += iovec[i].iov_len;
    return len;
}

void SocketImpl::send(
        const unsigned streamId,
        const void*    msg,
        const size_t   len)
{
    struct sctp_sndrcvinfo sinfo;
    sndrcvinfoInit(sinfo, streamId, len);
    int sendStatus;
    {
        std::unique_lock<std::mutex> lock(writeMutex);
        sendStatus = sctp_send(sock, msg, len, &sinfo, MSG_EOR);
    }
    checkIoStatus("sctp_send()", len, sendStatus);
}

void SocketImpl::sendv(
        const unsigned streamId,
        struct iovec*  iovec,
        const int      iovcnt)
{
    ssize_t numExpected = iovLen(iovec, iovcnt);
    struct {
        struct cmsghdr         cmsghdr;
        struct sctp_sndrcvinfo sinfo;
    } msg_control;
    msg_control.cmsghdr.cmsg_len = sizeof(msg_control);
    msg_control.cmsghdr.cmsg_level = IPPROTO_SCTP;
    msg_control.cmsghdr.cmsg_type = SCTP_SNDRCV;
    sndrcvinfoInit(msg_control.sinfo, streamId, numExpected);
    struct msghdr msghdr = {0};
    msghdr.msg_iov = iovec;
    msghdr.msg_iovlen = iovcnt;
    msghdr.msg_control = &msg_control;
    msghdr.msg_controllen = sizeof(msg_control);
    ssize_t sendStatus;
    {
        std::unique_lock<std::mutex> lock(writeMutex);
        sendStatus = sendmsg(sock, &msghdr, MSG_EOR);
    }
    checkIoStatus("sendmsg()", numExpected, sendStatus);
}

void SocketImpl::getNextMsgInfo()
{
    struct sctp_sndrcvinfo  sinfo;
    uint8_t                 msg;
    int                     flags = MSG_PEEK;
    int                     numRead;
    socklen_t               socklen = 0;
    {
        std::unique_lock<std::mutex> lock(readMutex);
        numRead = sctp_recvmsg(sock, &msg, sizeof(msg), nullptr, &socklen,
                &sinfo, &flags);
    }
    if (numRead == 0) {
        size = 0;
    }
    else {
        checkIoStatus("sctp_recvmsg()", sizeof(msg), numRead);
        streamId = sinfo.sinfo_stream;
        size = ntohl(sinfo.sinfo_ppid);
    }
    needMsg = false;
}

void SocketImpl::ensureMsg()
{
    if (needMsg)
        getNextMsgInfo();
}

uint32_t SocketImpl::getSize()
{
    ensureMsg();
    return size;
}

uint32_t SocketImpl::getStreamId()
{
    ensureMsg();
    return streamId;
}

void SocketImpl::recv(
        void*        msg,
        const size_t len,
        int          flags)
{
    struct sctp_sndrcvinfo  sinfo;
    int                     numRead;
    socklen_t               socklen = 0;
    {
        std::unique_lock<std::mutex> lock(readMutex);
        numRead = sctp_recvmsg(sock, msg, len, (struct sockaddr*)nullptr,
                &socklen, &sinfo, &flags);
    }
    checkIoStatus("sctp_recvmsg()", len, numRead);
    needMsg = true;
}

void SocketImpl::recvv(
        struct iovec*  iovec,
        const int      iovcnt,
        const int      flags)
{
    ssize_t numExpected = iovLen(iovec, iovcnt);
    struct msghdr msghdr = {0};
    msghdr.msg_iov = iovec;
    msghdr.msg_iovlen = iovcnt;
    ssize_t numRead;
    {
        std::unique_lock<std::mutex> lock(readMutex);
        numRead = recvmsg(sock, &msghdr, flags);
    }
    checkIoStatus("recvmsg()", numExpected, numRead);
    needMsg = true;
}

} // namespace