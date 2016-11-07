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
#ifndef SE3313_NETWORKING_SOCKET_HPP
#define SE3313_NETWORKING_SOCKET_HPP

#include <assert.h>
#include <cstring>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>

#include <stdexcept>
#include <vector>
#include <string>

namespace se3313 
{

namespace networking 
{

/**
 * Defines a port for a socket, a 16-bit uint is used as ports live between 0-65536.
 */
typedef uint16_t port_t;
  
class socket final
{

public:

    /// The maximum number of bytes that can be read at one time.
    static const size_t MAX_BUFFER_SIZE = 1024;

    /// Socket descriptor type (used in C)
    typedef int socket_desc_t;

private:

    ssize_t write(const char* const buff, const size_t length);

    sockaddr_in _socketDescriptor;
    socket_desc_t _socketFD;
    bool _open;

public:

    /*!
     * \brief Opens a socket against the `ipAddress` with `port`
     */
    socket(const std::string& ipAddress, const uint16_t port);
    
    /**
     * Opens a socket based on an existing file descriptor.
     */
    socket(const socket_desc_t socketFD);

    // We explicitly delete the copy constructor and operator= because sockets must be
    // "owned," copying a file descriptor doesn't make sense.
    socket(const socket& s) = delete;
    socket & operator=(socket const & s) = delete;

    ~socket();

    /// Get the underlying socket file descriptor
    inline
    socket_desc_t fd() const { return _socketFD; }
    
    /*!
     * \brief Returns true if the socket is open.
     * \return `true` if open
     */
    inline 
    const bool isOpen() const { return _open; }
    
    /// Closes a socket, closing the underlying file descriptor
    void close();

    /*!
     * \brief Writes the value in `buff` to the socket if open. 
     * Writes the value in `buff` to the socket if it is open, otherwise it will throw an error. 
     * \param buff The buffer to write to. 
     * \return Number of bytes written or -1 for an error or 0 for the socket closing.
     */
    inline
    ssize_t write(const char* const raw)
    {
        return this->write(raw, ::strlen(raw));
    }


    /**
     * Writes a string to the socket
     * 
     * @return -1 if an error, 0 if disconnected and the amount of bytes written otherwise.
     */
    inline
    ssize_t write(const std::string& str_buff)
    {
        return this->write(str_buff.c_str(), str_buff.size());
    }

    /**
     * Writes a @c vector of chars
     * 
     * @return -1 if an error, 0 if disconnected and the amount of bytes written otherwise.
     */
    inline
    ssize_t write(const std::vector<char>& vec)
    {
        const char* buff = reinterpret_cast<const char*>(vec.data());
        return this->write(buff, vec.size());
    }

    /**
     * Reads the socket data into a @c vector
     * 
     * @return -1 if an error, 0 if disconnected and the amount of bytes read otherwise.
     */
    ssize_t read(std::vector<char>* const vec);

    /**
     * Reads the socket data into a @c string directly
     * 
     * @return -1 if an error, 0 if disconnected and the amount of bytes read otherwise.
     */
    ssize_t read(std::string* const str);
};


} // end namespace networking

} // end namespace se3313



#endif // SE3313_NETWORKING_SOCKET_HPP
