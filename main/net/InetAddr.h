/**
 * This file declares a handle class for an immutable Internet address.
 *
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: InetAddr.h
 * @author: Steven R. Emmerson
 */

#ifndef INETADDR_H_
#define INETADDR_H_

#include <cstring>
#include <memory>
#include <netinet/in.h>

namespace hycast {

class InetAddr final
{
public:
    class Impl;

private:
    std::shared_ptr<Impl> pImpl;

    /**
     * Constructs from an implementation.
     * @param[in] impl  implementation
     */
    InetAddr(Impl* impl);

public:
    /**
     * Default constructs.
     */
    InetAddr();

    /**
     * Constructs from an IPv4 address in network byte order.
     * @param[in] addr  IPv4 address in network byte order
     */
    InetAddr(const struct in_addr& addr);

    /**
     * Constructs from an IPv6 address.
     * @param[in] addr  IPv6 address
     */
    InetAddr(const struct in6_addr& addr);

    /**
     * Constructs from a string representation of an Internet address.
     * @param[in] addr  String representation of Internet address
     */
    InetAddr(const std::string& addr);

    /**
     * Returns the `struct sockaddr_storage` corresponding to this instance and
     * a port number.
     * @param[out] storage   Storage structure
     * @param[in]  port      Port number in host byte order
     * @param[in]  sockType  Socket-type hint as for `socket()`. 0 =>
     *                       unspecified.
     * @exceptionsafety      Nothrow
     * @threadsafety         Safe
     */
    void setSockAddr(
            sockaddr_storage& storage,
            const int         port,
            const int         sockType = 0) const noexcept;

    /**
     * Returns the hash code of this instance.
     * @return This instance's hash code
     * @exceptionsafety Nothrow
     * @threadsafety    Safe
     */
    size_t hash() const noexcept;

    /**
     * Indicates if this instance is considered equal to another.
     * @param[in] that  Other instance
     * @retval `true`   Iff instance is equal to other
     */
    bool operator==(const InetAddr& that) const noexcept;

    /**
     * Indicates if this instance is not considered equal to another.
     * @param[in] that  Other instance
     * @retval `true`   Iff instance is not equal to other
     */
    bool operator!=(const InetAddr& that) const noexcept;

    /**
     * Indicates if this instance is considered less than another.
     * @param[in] that  Other instance
     * @retval `true`   Iff instance is less than the other
     */
    bool operator<(const InetAddr& that) const noexcept;

    /**
     * Indicates if this instance is considered less than or equal to another.
     * @param[in] that  Other instance
     * @retval `true`   Iff instance is less than or equal to other
     */
    bool operator<=(const InetAddr& that) const noexcept;

    /**
     * Indicates if this instance is considered greater than or equal to another.
     * @param[in] that  Other instance
     * @retval `true`   Iff instance is greater than or equal to other
     */
    bool operator>=(const InetAddr& that) const noexcept;

    /**
     * Indicates if this instance is considered greater than another.
     * @param[in] that  Other instance
     * @retval `true`   Iff instance is greater than other
     */
    bool operator>(const InetAddr& that) const noexcept;

    /**
     * Returns the string representation of the Internet address.
     * @return The string representation of the Internet address
     * @exceptionsafety Strong
     */
    std::string to_string() const;

    /**
     * Returns a new socket.
     * @param[in] sockType  Type of socket as defined in <sys/socket.h>:
     *                        - SOCK_STREAM     Streaming socket (e.g., TCP)
     *                        - SOCK_DGRAM      Datagram socket (e.g., UDP)
     *                        - SOCK_SEQPACKET  Record-oriented socket
     * @return Corresponding new socket
     * @throws std::system_error  `socket()` failure
     * @exceptionsafety  Strong guarantee
     * @threadsafety     Safe
     */
    int getSocket(const int sockType) const;

    /**
     * Sets the interface to use for outgoing datagrams.
     * @param[in] sd     Socket descriptor
     * @exceptionsafety  Strong guarantee
     * @threadsafety     Safe
     */
    void setInterface(const int sd) const;

    /**
     * Sets the hop-limit on a socket for outgoing multicast packets.
     * @param[in] sd     Socket
     * @param[in] limit  Hop limit:
     *                     -         0  Restricted to same host. Won't be
     *                                  output by any interface.
     *                     -         1  Restricted to the same subnet. Won't
     *                                  be forwarded by a router (default).
     *                     -    [2,31]  Restricted to the same site,
     *                                  organization, or department.
     *                     -   [32,63]  Restricted to the same region.
     *                     -  [64,127]  Restricted to the same continent.
     *                     - [128,255]  Unrestricted in scope. Global.
     * @throws std::system_error  `setsockopt()` failure
     * @returns  This instance
     * @execptionsafety  Strong guarantee
     * @threadsafety     Safe
     */
    void setHopLimit(
            const int      sd,
            const unsigned limit) const;

    /**
     * Sets whether or not a multicast packet written to a socket will be
     * read from the same socket. Such looping in enabled by default.
     * @param[in] sd      Socket descriptor
     * @param[in] enable  Whether or not to enable reception of sent packets
     * @return  This instance
     * @exceptionsafety  Strong guarantee
     * @threadsafety     Safe
     */
    void setMcastLoop(
            const int  sd,
            const bool enable) const;
};

} // namespace

#endif /* INETADDR_H_ */
