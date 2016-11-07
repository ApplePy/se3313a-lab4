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

#include <errno.h>

#include <unistd.h>

#include <cstring>
#include <iostream>
#include <stdexcept>
#include <sstream>

#include "networking/socket_server.hpp"

namespace net = se3313::networking;

net::socket_server::socket_server(const se3313::networking::port_t port)
{
    // The first call has to be to socket(). This creates a UNIX socket.
    _socketFD = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFD < 0)
    {
        throw std::runtime_error("Unable to open the socket server");
    }

    // The second call is to bind().  This identifies the socket file
    // descriptor with the description of the kind of socket we want to have.
    std::memset(&_socketDescriptor, sizeof(sockaddr_in), 0);
    _socketDescriptor.sin_family = AF_INET;
    _socketDescriptor.sin_port = htons(port);
    _socketDescriptor.sin_addr.s_addr = INADDR_ANY;
    
    // bind to the socket after intializing all code
    const int err = ::bind(_socketFD, (sockaddr*)&_socketDescriptor, sizeof(sockaddr_in));
    if (err < 0)
    {
        std::ostringstream ss; ss << "Unable to bind socket to requested port, err: " << errno;
        throw std::runtime_error(ss.str());
    }

    // Set up a maximum number of pending connections to accept
    ::listen(_socketFD, 20);

    // At this point, the object is initialized.  So return.
}

net::socket_server::~socket_server()
{
    this->close();
}


// This function will block forever.  It is probably best to use shutdown from a master thread to
// kill the call to accept.  But let's do it later.
std::shared_ptr<net::socket> net::socket_server::accept()
{
    int connectionFD = ::accept(_socketFD, NULL, 0);
    if (connectionFD < 0)
    {
        std::ostringstream ss; ss << "Socket error: " << errno;
        throw std::runtime_error(ss.str());
    }

    std::cout << __func__ << ": connectionFD:" << connectionFD << " socketFD:" << _socketFD << std::endl;
    return std::make_shared<net::socket>(connectionFD);
}

void net::socket_server::close()
{
    ::shutdown(_socketFD, SHUT_RDWR);
}
