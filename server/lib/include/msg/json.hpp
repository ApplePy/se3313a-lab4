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
#ifndef SE3313_MSG_UTILS_HPP
#define SE3313_MSG_UTILS_HPP

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>


namespace se3313
{

namespace msg
{

namespace json
{

/**
 * Converts a string into a ptree, but does not check for anything other than syntax.
 */
static inline
boost::property_tree::ptree from(const std::string& input) {
    boost::property_tree::ptree ptree;
    
    // clean the string first:
    std::string cpy = input;
    auto idx = cpy.rfind('\n');
    if (idx != std::string::npos) 
    {
        cpy = cpy.erase(idx+1, std::string::npos);
    }
    
    auto lastBraceIdx = cpy.rfind('}');
    if (lastBraceIdx != std::string::npos && lastBraceIdx != cpy.size()-1) 
    {
        cpy = cpy.erase(lastBraceIdx+1, std::string::npos);
    }

    std::istringstream ss;
    ss.str(cpy);
    
    
    boost::property_tree::read_json(ss, ptree);

    return ptree;
}

/**
 * Converts a ptree into a string.
 * @param ptree Tree to convert 
 * @param pretty If `true`, puts in indentation and newlines
 */
static inline
const std::string to(boost::property_tree::ptree ptree, const bool pretty = false)
{
    std::ostringstream ss;
    boost::property_tree::write_json(ss, ptree, pretty);
    return ss.str();
}
    
}

} // end namespace msg

} // end namespace se3313

#endif // SE3313_MSG_UTILS_HPP
