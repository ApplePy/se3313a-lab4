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

#include "msg/instance.hpp"

#include <boost/property_tree/ptree.hpp>

using namespace se3313;
using namespace msg;

constexpr const char instance::PROPERTY_TYPE[];
constexpr const char instance::PROPERTY_OBJECT[];

constexpr const char instance::PROPERTY_DATETIME[];
constexpr const char instance::PROPERTY_SENDER[];

constexpr const char instance::SERVER_SENDER[];
constexpr const char instance::UNKNOWN_SENDER[];

boost::optional<std::pair<std::string, boost::property_tree::ptree>>
instance::extractFrom(boost::property_tree::ptree json)
{
    typedef std::pair<std::string, boost::property_tree::ptree> ret_pair_t;
    
    const auto type_prop_it = json.find(msg::instance::PROPERTY_TYPE);
    const auto obj_prop_it = json.find(msg::instance::PROPERTY_OBJECT);
        
    if (type_prop_it == json.not_found())
    {   // invalid specification for the json, no "type" tag
        std::cerr << "No type tag specified (tag=\"" 
                  << msg::instance::PROPERTY_TYPE 
                  << "\")" 
                  << std::endl;
        return boost::none;
    } // fallthrough on else
        
    if (obj_prop_it == json.not_found())
    {
        std::cerr << "No object tag specified (tag=\"" 
                    << msg::instance::PROPERTY_OBJECT 
                    << "\")"
                    << std::endl;
        return boost::none;
    }
    
    std::string type = type_prop_it->second.get_value<std::string>();
    boost::property_tree::ptree obj = obj_prop_it->second;
    
    return boost::make_optional<ret_pair_t>(ret_pair_t({type, obj}));
}

