/**
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: Socket.cpp
 * @author: Steven R. Emmerson
 *
 * This file defines a RAII object for a socket.
 */

#include "Socket.h"

#include <unistd.h>

namespace hycast {

/**
 * Constructs from nothing.
 */
Socket::Socket()
    : sock(-1),
      sptr(new int)
{
}

/**
 * Constructs from a socket. Only do this once per socket because the destructor
 * might close the socket.
 * @see Socket::~Socket()
 * @see Socket::operator=(Socket& socket)
 * @see Socket::operator=(Socket&& socket)
 * @param[in] sock  The socket
 */
Socket::Socket(const int sock)
    : sock(sock),
      sptr(new int)
{
    if (sock < 0)
        throw std::invalid_argument("Invalid socket: " + std::to_string(sock));
}

/**
 * Constructs from another instance. The returned instance will close the socket
 * upon destruction if it's the last one that references the socket.
 * @see Socket::~Socket()
 * @see Socket::operator=(Socket& socket)
 * @see Socket::operator=(Socket&& socket)
 * @param[in] socket  The other instance.
 */
Socket::Socket(const Socket& socket)
    : sock(socket.sock),
      sptr(socket.sptr)
{
}

/**
 * Constructs from a temporary instance. Move constructor. The returned instance
 * will close the socket upon destruction if it's the last one that references
 * the socket.
 * @see Socket::~Socket()
 * @see Socket::operator=(Socket& socket)
 * @see Socket::operator=(Socket&& socket)
 * @param[in] socket  The other instance.
 */
Socket::Socket(const Socket&& socket)
    : sock(socket.sock),
      sptr(socket.sptr)
{
}

/**
 * Destroys an instance. Closes the underlying socket if this is the last
 * instance that references it.
 * @see Socket::Socket(int sock)
 * @see Socket::Socket(Socket& socket)
 * @see Socket::Socket(Socket&& socket)
 * @see Socket::operator=(Socket& socket)
 * @see Socket::operator=(Socket&& socket)
 */
Socket::~Socket()
{
    if (sptr.unique())
        (void)close(sock);
}

/**
 * Unconditionally assigns another instance to this instance. Closes this
 * instance's socket before the assignment if this instance holds the last
 * reference to it.
 * @param[in] that  The other instance
 */
void Socket::copy_assign(const Socket& that)
{
    if (sptr.unique())
        close(sock);
    sock = that.sock;
    sptr = that.sptr;
}

/**
 * Assigns from another instance. The returned instance will close the socket
 * upon destruction if it's the last one that references the socket.
 * @see Socket::Socket(int sock)
 * @see Socket::Socket(Socket& socket)
 * @see Socket::Socket(Socket&& socket)
 * @see Socket::~Socket()
 * @param[in] that  The other instance
 * @return This instance
 */
Socket& Socket::operator =(const Socket& that)
{
    if (this != &that)
        copy_assign(that);
    return *this;
}

/**
 * Assigns from a temporary instance. Move assignment operator. The returned
 * instance will close the socket upon destruction if it's the last one that
 * references the socket.
 * @see Socket::Socket(int sock)
 * @see Socket::Socket(Socket& socket)
 * @see Socket::Socket(Socket&& socket)
 * @see Socket::~Socket()
 * @param[in] that  The other instance
 * @return This instance
 */
Socket& Socket::operator =(const Socket&& that)
{
    copy_assign(that);
    return *this;
}

} // namespace
