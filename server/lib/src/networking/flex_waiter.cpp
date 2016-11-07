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

#include "networking/flex_waiter.hpp"

#include <iostream>
#include <stdio.h>
#include <sys/eventfd.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/unistd.h> 

#include <boost/assert.hpp>

using namespace se3313;
using namespace networking;

constexpr const uint64_t flex_waiter::KILL_SIGNAL;

flex_waiter::flex_waiter(std::shared_ptr<networking::socket_server> master) 
    : _killEventFD(-1)
    , _master(master) 
{
}

flex_waiter::flex_waiter() 
    : _killEventFD(-1)
    , _master(nullptr) 
{
}

flex_waiter::~flex_waiter()
{
    std::lock_guard<std::mutex> lock(_mut_killEvent);
    if (_killEventFD >= 0)
    {
        ::close(_killEventFD);
        _killEventFD = -1;
    }
}

void flex_waiter::addSocket(const std::shared_ptr<networking::socket> newSock)
{
    BOOST_ASSERT(newSock);
    
    _sockets.insert(newSock);
}

void flex_waiter::removeSocket(const std::shared_ptr<networking::socket> sock) 
{
    BOOST_ASSERT(sock);
    
    auto it = _sockets.find(sock);
    if (it != _sockets.end()) 
    {
        _sockets.erase(it);
    }
}

void flex_waiter::kill() 
{
    std::lock_guard<std::mutex> lock(_mut_killEvent);
    if (_killEventFD >= 0)
    {
        ::write(_killEventFD, &KILL_SIGNAL, sizeof(KILL_SIGNAL));
    }
}

void flex_waiter::wait(const std::shared_ptr<activity_visitor> handler, std::chrono::milliseconds timeout)
{
    BOOST_ASSERT_MSG(handler, "Must specify a valid handler.");
    
    // calculate the timeout
    ::timeval tv;
    ::timeval * pTimeout = &tv;

    if (timeout > std::chrono::seconds::min())
    {
        const auto v = timeout.count();
        
        tv.tv_sec = v/1000;
        tv.tv_usec = (v%1000)*1000;
    } 
    else 
    {
        pTimeout = NULL;
    }
    
    ::fd_set theSet;
    FD_ZERO(&theSet);
    
    FD_SET(STDIN_FILENO, &theSet);
    int maxFD = STDIN_FILENO; // stdin is LIKELY 0, but oh well.
    
    if (_master)
    {
        const int masterFD = _master->fd();
        maxFD = masterFD;
        FD_SET(masterFD, &theSet);
    }
        
    for (const auto& sock: _sockets)
    {
        if (sock->isOpen()) {
            const int fd = sock->fd();
            
            maxFD = std::max(maxFD, fd);
            FD_SET(fd, &theSet);
        }
    }
    
    int killEventFD;
    {
        // Scope lock the killEvent file descriptor, that way we can't close or change it
        // while its locked
        
        std::lock_guard<std::mutex> lock(_mut_killEvent);
        if (_killEventFD == -1)
        {
            // See `man eventfd(2)`
            const int efd = ::eventfd(0, 0);
            if (efd == -1) 
            {
                throw std::runtime_error("Could not create unlock event.");
            }
            
            _killEventFD = efd;
        }
        killEventFD = _killEventFD;
    }
    
    FD_SET(killEventFD, &theSet);
    maxFD = std::max(maxFD, killEventFD);
    
    const int retval = ::select(maxFD + 1, &theSet, NULL, NULL, pTimeout);

    if (retval < 0)
    {
        throw std::runtime_error("Unexpected error in synchronization object");
    } 
    else if (retval) 
    {
        // Check if someone killed externally
        if (FD_ISSET(killEventFD, &theSet)) 
        {
            std::cout << __func__ << ": Received kill signal." << std::endl;
            uint64_t killv;
            size_t killrv = ::read(killEventFD, (void*) &killv, sizeof(uint64_t));
            
            if (killrv > 0)
            {
                if (killrv != sizeof(uint64_t)) 
                {
                    throw std::runtime_error("Invalid signal read from kill, "
                                            "probably implementation problem.");
                } 
                else 
                {
                    return;
                }
            }
        } 
        else if (FD_ISSET(STDIN_FILENO, &theSet))
        {
            std::string input;
            
            char buff[BUFFER_SIZE];
            ::memset(buff, 0, BUFFER_SIZE);
            ssize_t n = ::read(STDIN_FILENO, buff, BUFFER_SIZE);
            
            if (n < 0)
            {
                throw std::runtime_error("Failed to read from stdin after being notified of being active.");
            }
            
            input = buff;
            if (!input.empty() && input.back() == '\n')
            {
                input.erase(input.end() - 1);
            }
            
            handler->onSTDIN(input);
        }
        else 
        {
            if (_master && FD_ISSET(_master->fd(), &theSet))
            {
                handler->onSocketServer(_master);
            } 
            else
            {
                for (const auto& child: _sockets)
                {
                    const int fd = child->fd();
                    if (FD_ISSET(fd,&theSet)) 
                    {
                        handler->onSocket(child);
                        break;
                    }
                }
                
                // If we don't find it, it just timed out *oh well*
            }
        } 
    } // end if (retval)
    
}
