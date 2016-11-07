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
#ifndef SE3313_MSG_ERROR_HPP
#define SE3313_MSG_ERROR_HPP

#include <boost/property_tree/ptree.hpp>

#include <cstddef>
#include <string>

#include "instance.hpp"

namespace se3313
{

namespace msg
{

/*!
 * \brief Describes error codes passed back to a client when there is an error within the server.
 */
enum class ErrorCode: uint16_t {
    NONE                    = 0,

    /**
     * Valid request, but invalid timing. 
     */
    INVALID_REQUEST_FROM_CLIENT = 50,
    
    
    /*!
     * The Username already exists on the server at this time.
     */
    USER_NAME_IN_USE        = 1,
    
    /*!
     * The User Name was invalid (e.g. empty).
     */
    INVALID_USER_NAME       = 2,

    MALFORMED_REQUEST_UNKNWN    = 200,

    MALFORMED_REQUEST_NO_TYPE       = 201,
    MALFORMED_REQUEST_UNKNOWN_TYPE  = 202,
    MALFORMED_REQUEST_BAD_OBJECT_DEF= 203,
    MALFORMED_REQUEST_NO_OBJ    = 210
};
    
    
namespace response {

/**
 * Describes an error from the server to a client.
 */
class error: public abstract_instance<error> {

public:
    
    /// Java-land type
    constexpr static const char TYPE[] = "ca.uwo.eng.se3313.lab4.network.response.ServerError";
    
    /// The error code 
    constexpr static const char PROPERTY_CODE[] = "code";
    /// A nice-ish description of the error
    constexpr static const char PROPERTY_MESSAGE[] = "message";
    /// Who sent the error originally
    constexpr static const char PROPERTY_ORIGINATOR[] = "originator";
    
    /**
     * Converts a json tree to an instance.
     */
    static 
    std::shared_ptr<error> fromJson(boost::property_tree::ptree& json);

    /**
     * Delegate constructor defaulting time to now and sender to the server.
     */
    error(const std::string& originator,
          const ErrorCode errorCode, 
          const std::string& message)
        : error(clock_t::now(), instance::SERVER_SENDER, originator, errorCode, message) 
        {}
       
    /**
     * Creates an error.
     * @param dateTime When the error occured
     * @param sender Who sent the error (usually server)
     * @param originator Who found the error (usually server again)
     * @param errorCode The code of the error
     * @param message Human readable message
     */
    error(const time_point_t dateTime, 
          const std::string& sender,
          const std::string& originator,
          const ErrorCode errorCode, 
          const std::string& message)
        : abstract_instance(dateTime, sender)
        , _originator(originator)
        , _errorCode(errorCode)
        , _message(message) 
        {}
    
    /// default destructor
    virtual ~error() = default;

    /// Get the original creator
    const std::string originator() const { return _originator; }
    
    /// Get the human-readable message
    const std::string message() const { return _message; }
    
    /// Get the error code
    const ErrorCode code() const { return _errorCode; }

protected:
    
    /// Get the subtree for the error
    virtual
    boost::property_tree::ptree subToJson() const;

private:

    const std::string _originator;
    
    const ErrorCode _errorCode;

    const std::string _message;

};

/**
 * Returns `true` if the message is an error, `false` if `nullptr` or otherwise.
 * 
 * @param instance @c instance to check against.
 * @return `true` if @p instance is an @c error instance.
 */
const bool is_error_msg(const std::shared_ptr<msg::instance> instance);

} // end namespace response

} // end namespace msg

} // end namespace se3313

#endif // SE3313_MSG_ERROR_HPP
