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
#ifndef SE3313_NETWORKING_MESSAGE_HPP_
#define SE3313_NETWORKING_MESSAGE_HPP_

#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>

#include <sstream>
#include <string>

#include "instance.hpp"

namespace se3313 {

namespace msg {

namespace request {

/**
 * Represents a message request from a client.
 */
class message: public abstract_instance<message> {

public:
    
    /// The type from Java-land
    constexpr static const char TYPE[] = "ca.uwo.eng.se3313.lab4.network.request.MessageRequest";
    
    /// Property for the payload
    constexpr static const char PROPERTY_CONTENT[] = "content";

    /// Converts a Json tree into a \c message. 
    static
    std::shared_ptr<message> fromJson(boost::property_tree::ptree& json);
    
    /**
     * Constructs a new instance of a message
     * @param dateTime When the message was sent
     * @param from Who sent the mssage
     * @param content The payload
     */
    message(const time_point_t dateTime, const std::string& from, const std::string& content)
        : abstract_instance(dateTime, from)
        , _content(content) {}
    
    /**
     * Delegate constructor, sets @p dateTime to the current server time. 
     */
    message(const std::string& from, const std::string& content)
        : message(clock_t::now(), from, content) 
        {}
    
    /// Defaulted constructor
    virtual ~message() = default;

    /**
     * Get the payload
     */
    const std::string content() const { return _content; }

protected:
    
    /**
     * Returns `this` instance's properties as a Json tree to be nested with other calls.
     */
    virtual
    boost::property_tree::ptree subToJson() const override;

private:

    /// Payload
    const std::string _content;

};

}

namespace response {

/**
 * Represents the response from the server to a message
 */
class message: public abstract_instance<message> {

public:
    
   /// The type from Java-land
    constexpr static const char TYPE[] = "ca.uwo.eng.se3313.lab4.network.response.MessageResponse";
    
    /// Property for the payload
    constexpr static const char PROPERTY_CONTENT[] = "content";
    
    /// Property for the originator
    constexpr static const char PROPERTY_ORIGINATOR[] = "originator";

    /// Converts a Json tree into a \c message. 
    static
    std::shared_ptr<message> fromJson(boost::property_tree::ptree& json);
    
    /**
     * Creates a new response.
     * @param dateTime When the message was sent out by the server
     * @param sender Who is sending the message (almost always the server)
     * @param originator Who sent the original message
     * @param content The message payload
     */
    message(const time_point_t dateTime, const std::string& sender, 
            const std::string& originator, const std::string& content)
        : abstract_instance(dateTime, sender)
        , _originator(originator)
        , _content(content) 
        {}
       
    /**
     * Delegate constructor which defaults the time to the current server time and
     * the sender to the server.
     */
    message(const std::string& originator, const std::string& content)
        : message(clock_t::now(), instance::SERVER_SENDER, originator, content)
        {}
    
    /// Default destructor
    virtual ~message() = default;

    /// Who sent the original message.
    const std::string originator() const { return _originator; }
    
    /// The payload of the message
    const std::string content() const { return _content; }
    
protected:

    virtual
    boost::property_tree::ptree subToJson() const override;

private:

    /// Who sent the original message
    const std::string _originator;
    
    /// Payload of the message
    const std::string _content;

};

}

}

}


#endif // SE3313_NETWORKING_MESSAGE_HPP_
