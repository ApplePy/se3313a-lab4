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

#include <boost/property_tree/ptree.hpp>

#include <string>

#include "msg/error.hpp"
#include "msg/instance.hpp"

using namespace se3313;
using namespace msg;
using namespace response;

constexpr const char error::TYPE[];

// Properties
constexpr const char error::PROPERTY_CODE[];
constexpr const char error::PROPERTY_MESSAGE[];
constexpr const char error::PROPERTY_ORIGINATOR[];

boost::property_tree::ptree error::subToJson() const
{
    boost::property_tree::ptree json = baseJson();
    
    json.put(PROPERTY_CODE, static_cast<uint16_t>(_errorCode));
    json.put(PROPERTY_MESSAGE, _message);
    json.put(PROPERTY_ORIGINATOR, _originator);
    
    return json;
}

std::shared_ptr<error> error::fromJson(boost::property_tree::ptree& json)
{
    std::string sender;
    time_point_t dateTime;
    abstract_instance::extractCommonParams(json, &dateTime, &sender);
    
    // get all of the other parameters
    const auto codeIt = json.find(PROPERTY_CODE);
    if (codeIt == json.not_found())
    {
        throw std::runtime_error("Can not get error code from JSON");
    }
    const ErrorCode code = static_cast<ErrorCode>(codeIt->second.get_value<uint16_t>());
    
    const auto msgIt = json.find(PROPERTY_MESSAGE);
    if (msgIt == json.not_found())
    {
        throw std::runtime_error("Can not get message from JSON");
    }
    const std::string message = msgIt->second.get_value<std::string>();
    
    const auto origIt = json.find(PROPERTY_ORIGINATOR);
    if (origIt == json.not_found())
    {
        throw std::runtime_error("Can not get originator from JSON");
    }
    const std::string originator = origIt->second.get_value<std::string>();
    
    return std::make_shared<error>(dateTime, sender, originator, code, message);
}


const bool msg::response::is_error_msg(const std::shared_ptr<msg::instance> instance)
{
    const auto err = std::dynamic_pointer_cast<error>(instance);
    
    return !!err;
}