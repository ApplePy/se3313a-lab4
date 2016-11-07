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

#include "msg/message.hpp"

using namespace se3313;
using namespace msg;

namespace pt = boost::property_tree;

constexpr const char request::message::TYPE[];

constexpr const char request::message::PROPERTY_CONTENT[];

std::shared_ptr<request::message> request::message::fromJson(pt::ptree& json)
{
    std::string sender;
    time_point_t dateTime;
    abstract_instance::extractCommonParams(json, &dateTime, &sender);
    
    const auto messageIt = json.find(PROPERTY_CONTENT);
    if (messageIt == json.not_found())
    {
        throw std::runtime_error("Could not get message");
    }
    
    const std::string message = messageIt->second.get_value<std::string>();
    
    return std::make_shared<request::message>(dateTime, sender, message);
}

pt::ptree request::message::subToJson() const
{
    boost::property_tree::ptree json = baseJson();
    
    json.put(PROPERTY_CONTENT, _content);
    
    return json;
}

constexpr const char response::message::TYPE[];
constexpr const char response::message::PROPERTY_ORIGINATOR[];
constexpr const char response::message::PROPERTY_CONTENT[];

pt::ptree response::message::subToJson() const
{   
    boost::property_tree::ptree json = baseJson();
    
    json.put(PROPERTY_ORIGINATOR, _originator);
    json.put(PROPERTY_CONTENT, _content);
    
    return json;
}

std::shared_ptr<response::message> response::message::fromJson(pt::ptree& json)
{
    std::string sender;
    time_point_t dateTime;
    abstract_instance::extractCommonParams(json, &dateTime, &sender);
    
    const auto messageIt = json.find(PROPERTY_CONTENT);
    if (messageIt == json.not_found())
    {
        throw std::runtime_error("Could not get message.");
    }
    
    const std::string message = messageIt->second.get_value<std::string>();
    
    const auto origIt = json.find(PROPERTY_ORIGINATOR);
    if (origIt == json.not_found())
    {
        throw std::runtime_error("Could not get originator.");
    }
    
    const std::string originator = origIt->second.get_value<std::string>();
    
    return std::make_shared<response::message>(dateTime, sender, originator, message);
}
