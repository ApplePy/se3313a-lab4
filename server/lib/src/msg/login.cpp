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

#include "msg/login.hpp"

using namespace se3313;
using namespace msg;

namespace pt = boost::property_tree;

constexpr const char request::login::TYPE[];

std::shared_ptr<request::login> request::login::fromJson(pt::ptree& json)
{
    std::string sender;
    time_point_t dateTime;
    abstract_instance::extractCommonParams(json, &dateTime, &sender);
    
    return std::make_shared<login>(dateTime, sender);
}

pt::ptree request::login::subToJson() const
{
    return baseJson();
}


constexpr const char response::login::TYPE[];
constexpr const char response::login::PROPERTY_JOINING_USERNAME[];

pt::ptree response::login::subToJson() const
{   
    boost::property_tree::ptree json = baseJson();
    
    json.put(PROPERTY_JOINING_USERNAME, this->joiningUsername());
    
    return json;
}

std::shared_ptr<response::login> response::login::fromJson(pt::ptree& json)
{
    std::string sender;
    time_point_t dateTime;
    abstract_instance::extractCommonParams(json, &dateTime, &sender);
    
    auto joiningUserIt = json.find(PROPERTY_JOINING_USERNAME);
    if (joiningUserIt == json.not_found())
    {
        throw std::runtime_error("Can not get joining username from JSON");
    }
    const std::string joiningUser = joiningUserIt->second.get_value<std::string>();
    
    return std::make_shared<login>(dateTime, sender, joiningUser);
}
