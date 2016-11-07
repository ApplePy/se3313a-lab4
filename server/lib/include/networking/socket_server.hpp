/*
 * Copyright (c) 2016 Kevin Brightwell, Kenneth McIsaac
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#pragma once
#ifndef SE3313_NETWORKING_SOCKETSERVER_HPP
#define SE3313_NETWORKING_SOCKETSERVER_HPP

#include "socket.hpp"

#include <memory>

namespace se3313 
{

namespace networking
{
  
/**
 * Behaves as a "server" for sockets that listens for connections, when they occur, a @c socket
 * can be returned.
 */
class socket_server final
{
public:
    /// Convenience typedef from @c socket
    typedef socket::socket_desc_t socket_desc_t;

    /// Creates a new "server" waiting on @p port. 
    socket_server(const port_t port);
    
    /// Destructs the server, closing sockets.
    ~socket_server();

    /**
     * <strong>Blocking</strong> operation that waits until there is a connection and 
     * returns a @c socket upon connecting. 
     */ 
    std::shared_ptr<socket> accept();
    
    /// Closes the socket_server, it will unblock `accept()` calls.
    void close();
    
    /// Get the underlying socket descriptor. 
    socket_desc_t fd() const { return _socketFD; }
    
private:
    
    socket_desc_t _socketFD;
    
    sockaddr_in _socketDescriptor;
};

} // end namespace networking

} // end namespace se3313

#endif // SE3313_NETWORKING_SOCKETSERVER_HPP
