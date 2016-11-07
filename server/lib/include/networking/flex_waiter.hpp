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
#ifndef SE3313_NETWORKING_FLEXWAIT_HPP_
#define SE3313_NETWORKING_FLEXWAIT_HPP_

#include "socket.hpp"
#include "socket_server.hpp"

#include <sys/signal.h>

#include <mutex>
#include <memory>
#include <unordered_set>

namespace se3313 {
    
namespace networking {

/**
 * The flex_waiter utilizes the <a href="http://man7.org/linux/man-pages/man2/select.2.html">select()</a>
 * system call. Users will register sockets by calling `addSocket()` and passing the @c socket_server into the 
 * constructor. 
 * 
 * When calling `wait()` if there is any activity on any of the @c socket, the @c socket_server or @c std::cin, 
 * the `wait()` returns and calls the appropriate visit method on the @c flex_waiter::activity_visitor implementation
 * passed. 
 * 
 * This allows a single thread to multiplex against many different input functions. 
 */
class flex_waiter final
{
    
public:
    
    /// convenience typedef for @c networking::socket_server
    typedef std::shared_ptr<networking::socket_server> socket_server_ptr_t;
    
    /// convenience typedef for @c networking::socket
    typedef std::shared_ptr<networking::socket> socket_ptr_t;
    
    /**
     * Defines the behaviour for the "result" of calling `flex_waiter::wait()`.
     */
    class activity_visitor {
        
    public:
        
        /// Default destructor
        virtual ~activity_visitor() = default;
        
        /// Called when there is activity on the @c socket_server and passes the 
        /// instance.
        virtual
        void onSocketServer(const std::shared_ptr<networking::socket_server>) {}
        
        /// Called when there is activity on a @c socket.
        virtual
        void onSocket(const socket_ptr_t) = 0;
        
        /// Called from activity on `stdin`, passing the line of input received. 
        virtual
        void onSTDIN(const std::string& line) = 0;
    };
    
    /// The signal used to tell `select()` call internally to sto break out.
    constexpr static const uint64_t KILL_SIGNAL = SIGKILL;
    
    /// Buffer size used as needed. 
    constexpr static const size_t BUFFER_SIZE = 1024;
    
    /**
     * Convenience function to create a flex_waiter with a @c socket_server and multiple @c socket instances.
     * 
     * @param master @c socket_server in use.
     * @param sockets Variadic amount of @c socket instances.
     */
    template<class... Args>
    flex_waiter(std::shared_ptr<networking::socket_server> master, Args... sockets)
        : flex_waiter(master)
    {
        _sockets = std::unordered_set<socket_ptr_t>({ sockets... });
    }
    
    /// Creates an instance with just a @c socket_server and STDIN
    flex_waiter(std::shared_ptr<networking::socket_server> master);
    
    /// Creates an instance with no socket instances.
    flex_waiter();
    
    /// Destroys the waiter and frees required memory.
    virtual ~flex_waiter();
    
    /**
     * Wait on all of the sockets known <em>and</em> stdin. If activity is found, it calls the appropriate
     * virtual method.
     * 
     * This method is NOT thread-safe, you can not call this method on the <em>same</em> instance from multiple
     * threads. It is however safe to call ::kill() from another thread. 
     */
    void wait(const std::shared_ptr<activity_visitor> handler, std::chrono::milliseconds timeout = std::chrono::milliseconds::min());
    
    /**
     * Kills the currently waiting thread. Throws a runtime_error if there is no thread waiting.
     */
    void kill();
    
    /**
     * Adds a socket to calls to `wait()`. 
     * @param newSock socket to wait on
     */
    void addSocket(const socket_ptr_t newSock);
    
    /**
     * Removes a socket from the waiting set.
     * 
     * @param sock socket to remove 
     */
    void removeSocket(const socket_ptr_t sock);
    
    /// Sets the server to be a new value. 
    void setServer(const socket_server_ptr_t server) {
        _master = server;
    }
    

private:
    
    /// Mutex for the killEvent so it can be accessed from multiple threads
    std::mutex _mut_killEvent;
    int _killEventFD;
    
    /// Set of Socket pointers
    std::unordered_set<socket_ptr_t> _sockets;
    
    /// Pointer to a @c networking::socket_server instance
    socket_server_ptr_t _master;
};

} // end namespace networking

} // end namespace se3313

#endif // SE3313_NETWORKING_FLEXWAIT_HPP_