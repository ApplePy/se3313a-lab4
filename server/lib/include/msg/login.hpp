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
#ifndef SE3313_NETWORKING_LOGIN_HPP
#define SE3313_NETWORKING_LOGIN_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>

#include <sstream>
#include <string>

#include "instance.hpp"

namespace se3313 {

namespace msg {

namespace request {

/**
 * Represents a login request.
 */
class login: public abstract_instance<login> {

public:
   
    /// Java-land type
    constexpr static const char TYPE[] = "ca.uwo.eng.se3313.lab4.network.request.LoginRequest";

    /// Converts a ptree to a \c login.
    static
    std::shared_ptr<login> fromJson(boost::property_tree::ptree& json);
    
    /**
     * Creates an instance of \c login
     * 
     * @param datetime When the request occurred
     * @param username Who is requesting to connect 
     */
    login(const time_point_t datetime, const std::string& username)
        : abstract_instance(datetime, username) {}
      
    /// Delegate constructor defaulting the time to `now()`.  
    login(const std::string& username)
        : login(clock_t::now(), username) {}
    
    /// Default destructor
    virtual ~login() = default;

protected: 
    
    virtual
    boost::property_tree::ptree subToJson() const override;

};

}

namespace response {

/**
 * Represents a login response.
 */
class login: public abstract_instance<login> {

public:
    
    /// Java-land type
    constexpr static const char TYPE[] = "ca.uwo.eng.se3313.lab4.network.response.LoginResponse";
    
    /// States who just joined
    constexpr static const char PROPERTY_JOINING_USERNAME[] = "joiningUsername";

    /**
     * Converts a ptree into a \c login.
     */
    static
    std::shared_ptr<login> fromJson(boost::property_tree::ptree& json);
    
    /**
     * Creates a login response.
     * 
     * @param datetime the response time 
     * @param username The username who joined
     */
    login(const std::string& username)
        : login(clock_t::now(), instance::SERVER_SENDER, username) {}
        
    /**
     * Creates a login response.
     * 
     * @param datetime the response time 
     * @param sender The sending party, usually the server
     * @param username The username who joined
     */
    login(const time_point_t datetime, const std::string& sender, const std::string& username)
        : abstract_instance(datetime, sender)
        , _username(username) {}
    
    /// Default destructor
    virtual ~login() = default;

    /**
     * The sender, in this case the server.
     */
    const std::string sender() const override { return instance::SERVER_SENDER; }

    /**
     * The username who joined
     */
    const std::string joiningUsername() const { return _username; }
    
protected:
    
    virtual
    boost::property_tree::ptree subToJson() const override;

private:

    const std::string _username;

};

}

}

}


#endif // SE3313_NETWORKING_LOGIN_HPP
