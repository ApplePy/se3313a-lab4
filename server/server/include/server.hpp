


#ifndef JSMITH2_SERVER_HPP
#define JSMITH2_SERVER_HPP


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



namespace jsmith2 
{
    
    
class server final
{

public:

    typedef se3313::networking::port_t port_t;
    
private:
    
    const port_t _serverPort;
    
public:

    inline
    server(const port_t serverPort)
        : _serverPort(serverPort)
    { }

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
    

};

} // end namespace

#endif // JSMITH2_SERVER_HPP
