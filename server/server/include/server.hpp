


#ifndef DMURRA47_SERVER_HPP
#define DMURRA47_SERVER_HPP


#include <msg/instance.hpp>
#include <msg/error.hpp>
#include <msg/visitor.hpp>

#include <networking/flex_waiter.hpp>
#include <networking/socket.hpp>
#include <networking/socket_server.hpp>

#include <string>
#include <vector>

#include <atomic>
#include <condition_variable>
#include <deque>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <unordered_set>

#include <boost/bimap.hpp>
#include <boost/bimap/unordered_set_of.hpp>
#include <boost/optional.hpp>

#include "dmurra47_visitor.hpp"


namespace dmurra47 
{
    
    
class server final
{

public:

    typedef se3313::networking::port_t port_t;
    typedef se3313::networking::flex_waiter flex;
    typedef dmurra47::dmurra47_visitor visit;
    
private:
    
    const port_t _serverPort;
    
    flex waiter;		// Holds all the sockets
    std::thread* listenThread;	// Listens for connections/data and responds.
    visit visitor;		// Holds class instantiation for visitor.
    bool killed;		// Holds whether or not server.stop() was called.
    
public:

    inline
    server(const port_t serverPort)
        : _serverPort(serverPort)
    { 
      killed = false;
    }

    ~server();

    /*!
     * \brief Start the server.
     */
    void start();

    /*!
     * \brief Stop the server.
     */
    void stop();
    
    

private:
    
    // INSERT YOUR OPERATIONS BELOW
  void listen();

};

} // end namespace

#endif // DMURRA47_SERVER_HPP
