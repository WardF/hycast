/**
 * This file declares a client-side SCTP socket.
 *
 * Copyright 2016 University Corporation for Atmospheric Research. All rights
 * reserved. See the file COPYING in the top-level source-directory for
 * licensing conditions.
 *
 *   @file: ClientSocket.h
 * @author: Steven R. Emmerson
 */

#ifndef CLIENTSOCKET_H_
#define CLIENTSOCKET_H_

#include "InetSockAddr.h"
#include "Socket.h"

namespace hycast {

class ClientSocket final : public Socket {
public:
    /**
     * Constructs from an Internet socket address and the number of SCTP
     * streams.
     * @param[in] addr        Internet socket address
     * @param[in] numStreams  Number of SCTP streams
     */
    ClientSocket(
            const InetSockAddr& addr,
            const uint16_t      numStreams);
};

} // namespace

#endif /* CLIENTSOCKET_H_ */