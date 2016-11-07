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

#include "networking/socket.hpp"

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <assert.h>
#include <unistd.h>

#include <boost/assert.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <sstream>


namespace net = se3313::networking;

net::socket::socket(const std::string& ipAddress, const uint16_t port)
    : _open(false)
{
    // First, call socket() to get a socket file descriptor
    _socketFD = ::socket(AF_INET, SOCK_STREAM, 0);
    if (_socketFD < 0) 
    {
        throw std::runtime_error("Unable to initialize socket server");
    }

    // Start by zeroing out the socket descriptor
    std::memset(&_socketDescriptor, sizeof(sockaddr_in), 0);

    // Now try to map the IP address, as provided, into the socket Descriptor
    if (!inet_aton(ipAddress.c_str(), &_socketDescriptor.sin_addr))
    {
        throw std::runtime_error("IP Address provided is invalid");
    }

    _socketDescriptor.sin_family = AF_INET;
    _socketDescriptor.sin_port = htons(port);
    
    socket::socket_desc_t connectReturn = connect(_socketFD,(sockaddr*)&_socketDescriptor,sizeof(_socketDescriptor));
    if (connectReturn != 0)
    {
        throw std::runtime_error("Unable to open connection");
    }

    _open = true;
}

net::socket::socket(const socket_desc_t sFD)
    : _socketFD(sFD)
    , _open(true)
{ }

net::socket::~socket()
{
    if (this->_open) 
    {
        this->close();
    }
}

ssize_t net::socket::read(std::vector<char>* const vec)
{
    BOOST_ASSERT(vec);

    if (!_open)
    {
        throw std::runtime_error("Can not read from closed socket.");
    }

    char raw_buff[MAX_BUFFER_SIZE];
    std::memset(raw_buff, MAX_BUFFER_SIZE, 0);

    ssize_t received = ::recv(_socketFD, raw_buff, MAX_BUFFER_SIZE, 0);
    
    if (received == -1)
    {
        _open = false;
        std::cout << "Failed to read from socket (" << _socketFD << ")." << std::endl;
    } 
    else if (received == 0) 
    {
        _open = false;
        std::cout << __func__ << "@L" << __LINE__ << " Socket closed (" << _socketFD << ")." << std::endl;
    }
    else 
    {
        vec->clear();
        vec->reserve(received);
        vec->insert(vec->begin(), raw_buff, raw_buff + received);
    }

    return received;
}

ssize_t net::socket::read(std::string* const str)
{
    BOOST_ASSERT(str);

    if (!_open)
    {
        std::cout << __func__ << ": Tried to read form closed socket." << std::endl;
        return -1;
    }

    char raw_buff[MAX_BUFFER_SIZE];
    std::memset(raw_buff, MAX_BUFFER_SIZE, 0);

    ssize_t received = ::recv(_socketFD, raw_buff, MAX_BUFFER_SIZE, 0);

    // Return Value
    // These calls return the number of bytes received, or -1 if an error occurred. 
    // The return value will be 0 when the peer has performed an orderly shutdown. 
    
    if (received == -1)
    {
        _open = false;
        std::cout << __func__ << "@L" << __LINE__ << " Failed to read from socket (" << _socketFD << ")." << std::endl;
    } 
    else if (received == 0) 
    {
        _open = false;
        std::cout << __func__ << "@L" << __LINE__ << " Socket closed (" << _socketFD << ")." << std::endl;
    }
    else
    {
        *str = std::string(raw_buff);
    }

    return received;
}

ssize_t net::socket::write(const char* const buff, const size_t length)
{
    if (!this->_open)
    {
        throw std::runtime_error("Can not write to closed socket.");
    }

    ssize_t ret = ::write(this->_socketFD, buff, length);
    if (ret == -1)
    {
        this->_open = false;
        std::cout << "Socket failed to write." << std::endl;
    }
    
    return ret;
}

void net::socket::close()
{
    if (_open) 
    {
        _open = false;
        ::close(_socketFD);
    }
}
