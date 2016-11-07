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
#ifndef SE3313_MSG_VISITOR_HPP_
#define SE3313_MSG_VISITOR_HPP_

#include "error.hpp"
#include "instance.hpp"
#include "json.hpp"
#include "login.hpp"
#include "message.hpp"

#include <boost/property_tree/ptree.hpp>
#include <boost/optional.hpp>

#include <functional>
#include <sstream>
#include <string>
#include <unordered_map>


namespace se3313
{

namespace msg 
{

namespace request {

/**
 * Represents a base type that "accepts" a json tree and calls a method based on what
 * type the tree represents. 
 * 
 * @param R The return type for all of the visit* methods.
 */
template <typename R = std::shared_ptr<se3313::msg::instance>>
class abstract_message_visitor {

public:
    
    /// The result type
    typedef R return_t;

    /// No-arg constructor, initalizes internal components.
    abstract_message_visitor();

    /// Default destructor
    virtual ~abstract_message_visitor() = default;

    /// Called when a login message is passed
    virtual 
    return_t visitLogin(const request::login& /* request */ ) {
        return return_t();
    }

    /// Called when a message is passed
    virtual 
    return_t visitMessage(const request::message& /* request */ ) {
        return return_t();
    }
    
    /// Called when an error occurs
    virtual 
    return_t error(const std::string& /*originator*/,const ErrorCode /*code*/, const std::string& /*message*/)
    {
        return return_t();
    }
 
    /**
     * Provided function that will run the appropriate visitX function.
     *
     * \param json Tree structure parsed from a request
     */
    return_t visit(boost::property_tree::ptree& json)
    {
        const auto params = msg::instance::extractFrom(json);
        if (!params.is_initialized())
        {
            std::ostringstream ss; 
            ss << "Invalid request specified (json=\"";
            boost::property_tree::write_json(ss, json, false);
            ss << "\")"; 
            return this->error(instance::UNKNOWN_SENDER, ErrorCode::MALFORMED_REQUEST_NO_TYPE, ss.str());
        }
        
        const std::string type = params->first;
        boost::property_tree::ptree obj = params->second;
        
        const auto visit_it = _propMap.find(type);
        if (visit_it != _propMap.end()) {
            return visit_it->second(obj);
        } else {
            std::ostringstream ss; 
            ss << "Invalid object type tag specified (type=\"" << type << "\")"; 
            return this->error(instance::UNKNOWN_SENDER, ErrorCode::MALFORMED_REQUEST_NO_TYPE, ss.str());
        }
    }

private: 

    /// Stores a mapping between TYPE and json tree extraction.
    std::unordered_map<std::string, std::function<return_t(boost::property_tree::ptree&)>> _propMap;

};
  

template <typename R>
abstract_message_visitor<R>::abstract_message_visitor()
{
    namespace pt = boost::property_tree;
    
    auto onError = [this](const std::string& type, pt::ptree& json) -> R
    {
        std::ostringstream ss;
        ss <<  "Object was incorrectly defined for " << type << ", json=" << msg::json::to(json);
        return this->error(instance::UNKNOWN_SENDER, ErrorCode::MALFORMED_REQUEST_BAD_OBJECT_DEF, ss.str());
    };
    
    _propMap[request::login::TYPE] =
        [this, onError](pt::ptree& json) {
            const std::shared_ptr<request::login> oVal = request::login::fromJson(json);
            
            if (oVal)
            {
                return this->visitLogin(*oVal);
            }
            else
            {
                return onError("login", json);
            }
        };
        
    _propMap[request::message::TYPE] =
        [this, onError](pt::ptree& json) {
            const std::shared_ptr<request::message> oVal = request::message::fromJson(json);
            
            if (oVal)
            {
                return this->visitMessage(*oVal);
            }
            else
            {
                return onError("message", json);
            }
        };
}

} // end request 

namespace response {
    
/**
 * Represents a base type that "accepts" a json tree and calls a method based on what
 * type the tree represents. 
 * 
 * @param R The return type for all of the visit* methods.
 */
template <typename R = void>
class abstract_message_visitor {

public:
    
    /// The result type
    typedef R return_t;

    /// No-arg constructor, initalizes internal components.
    abstract_message_visitor();

    /// Default destructor
    virtual ~abstract_message_visitor() = default;

    /// Called when a login message is passed
    virtual 
    return_t visitLogin(const response::login& /* request */ ) {
        return return_t();
    }
    
    /// Called when a message response is found
    virtual 
    return_t visitMessage(const response::message& /* request */ ) {
        return return_t();
    }

    /// Called when an error message is required
    virtual 
    return_t error(const std::string& /*originator*/, const ErrorCode /*code*/, const std::string& /*message*/)
    {
        return return_t();
    }
 
    /**
     * Provided function that will run the appropriate visitX function.
     *
     * \param json Tree structure parsed from a request
     */
    return_t visit(boost::property_tree::ptree& json)
    {
        const auto params = msg::instance::extractFrom(json);
        if (!params.is_initialized())
        {
            std::ostringstream ss; 
            ss << "Invalid response specified (json=\"";
            boost::property_tree::write_json(ss, json, false);
            ss << "\")"; 
            return this->error(instance::UNKNOWN_SENDER, ErrorCode::MALFORMED_REQUEST_NO_TYPE, ss.str());
        }
        
        const std::string type = params->first;
        boost::property_tree::ptree obj = params->second;
        
        const auto visit_it = _propMap.find(type);
        if (visit_it != _propMap.end()) {
            return visit_it->second(obj);
        } else {
            std::ostringstream ss; 
            ss << "Invalid object type tag specified (type=\"" << type << "\")"; 
            return this->error(instance::UNKNOWN_SENDER, ErrorCode::MALFORMED_REQUEST_NO_TYPE, ss.str());
        }
    }

private: 

    /// Stores mapping between a TYPE and a parsing function
    std::unordered_map<std::string, std::function<return_t(boost::property_tree::ptree&)>> _propMap;

};
    
template <typename R>
abstract_message_visitor<R>::abstract_message_visitor()
{
    namespace pt = boost::property_tree;
    
    auto onError = [this](const std::string& type, pt::ptree& json) -> R
    {
        std::ostringstream ss;
        ss <<  "Object was incorrectly defined for " << type << ", json=" << msg::json::to(json);
        return this->error(instance::UNKNOWN_SENDER, ErrorCode::MALFORMED_REQUEST_BAD_OBJECT_DEF, ss.str());
    };
    
    _propMap[response::login::TYPE] =
        [this, onError](pt::ptree& json) {
            const std::shared_ptr<response::login> oVal = response::login::fromJson(json);
            
            if (oVal)
            {
                return this->visitLogin(*oVal);
            }
            else
            {
                return onError("login", json);
            }
        };
        
    _propMap[response::message::TYPE] =
        [this, onError](pt::ptree& json) {
            const std::shared_ptr<response::message> oVal = response::message::fromJson(json);
            
            if (oVal)
            {
                return this->visitMessage(*oVal);
            }
            else
            {
                return onError("message", json);
            }
        };
}

    
} // end namespace response

} // end namespace msg

} // end namespace se3313


#endif // SE3313_MSG_VISITOR_HPP_
