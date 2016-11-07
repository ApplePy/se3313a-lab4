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


#ifndef SE3313_NETWORK_INSTANCE_HPP_
#define SE3313_NETWORK_INSTANCE_HPP_

#include <chrono>
#include <chrono_io>
#include <sstream>
#include <string>

#include <boost/optional.hpp>

#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>

namespace se3313
{

namespace msg 
{

/**
 * Base interface for all messaging instances.
 */
class instance {

public:

    /// The clock used for timing
    typedef std::chrono::system_clock clock_t;
    
    /// Defines a point in time, this is "clock-dependant"
    // http://en.cppreference.com/w/cpp/chrono/time_point
    typedef clock_t::time_point time_point_t;
   
    /// The field in JSON where the type is stored
    constexpr static const char PROPERTY_TYPE[] = "type";

    /// The object field
    constexpr static const char PROPERTY_OBJECT[] = "object";
    
    /// Who sent the message
    constexpr static const char PROPERTY_SENDER[] = "sender";
    
    /// When the message was sent, a timestamp
    constexpr static const char PROPERTY_DATETIME[] = "datetime";
    
    /// Server sender tag
    constexpr static const char SERVER_SENDER[] = "@server";
    
    /// Unknown sender
    constexpr static const char UNKNOWN_SENDER[] = "@unknown";
    
    /**
     * Converts a json tree to the type <T>
     */
    template <typename T>
    static
    std::shared_ptr<T> fromJson(const boost::property_tree::ptree& json);
    
    /**
     * Get the sender of a message.
     * \return Username who sent a message. 
     */
    virtual const std::string sender() const = 0;
    
    /**
     * The time that a message was sent
     */
    virtual const time_point_t dateTime() const = 0;
    
    /// Default destructor
    virtual ~instance() = default;
    
    /**
     * Converts the current instance into a JSON tree. 
     */
    virtual
    boost::property_tree::ptree toJson() const = 0;
    
     /**
     * Tries to get data from a ptree.
     */
    static 
    boost::optional<std::pair<std::string, boost::property_tree::ptree>>
    extractFrom(boost::property_tree::ptree tree);

protected:  
    
    /**
     * Returns `this` instance's properties as a Json tree to be nested with other calls.
     */
    virtual
    boost::property_tree::ptree subToJson() const = 0;
    
    
};

inline
std::ostream& operator<<(std::ostream& os, const instance& inst)
{
    boost::property_tree::write_json(os, inst.toJson(), false);
    
    return os;
}
    
/**
 * Base abstract implementation of most of the methods in \c instance. This is the ideal
 * class to inherit from when working with instance types. 
 * 
 * @param S The type that inherits this type, this is an example of the 
 *          <a href="https://en.wikipedia.org/wiki/Curiously_recurring_template_pattern">Curiously recurring template pattern</a>
 *          which makes the subtypes easier to work with. Always make sure S is the subtype. 
 */    
template <typename S>
class abstract_instance : public instance {
    
public:
  
    /**
     * Creates an instance
     * @param dateTime When the instance was created
     * @param sender Who sent the instance
     */
    abstract_instance(const time_point_t dateTime, const std::string& sender)
        : _sender(sender)
        , _dateTime(dateTime) {}
    
    /// Default destructor
    virtual ~abstract_instance() = default;
    
    /// Who sent the message
    virtual 
    const std::string sender() const override { return _sender; }
    
    /// When the message was sent
    virtual 
    const time_point_t dateTime() const override { return _dateTime; }
    
    /**
     * Converts the current instance into a JSON tree.
     */
    boost::property_tree::ptree toJson() const final;
    
protected:
    
    // Converts the underlying fields into a Json tree
    virtual
    boost::property_tree::ptree subToJson() const override = 0;
    
    /// The sender of the message
    const std::string _sender;
    
    /// When the message was sent
    const time_point_t _dateTime;
    
    /// Puts the common parameters in this type into a ptree.
    boost::property_tree::ptree baseJson() const;
    
    /// Does the converse of \m baseJson, gets the params from a tree. 
    static 
    void extractCommonParams(boost::property_tree::ptree& json, time_point_t* const time, std::string* const sender);
    
};

template <typename T>
std::shared_ptr<T> instance::fromJson(const boost::property_tree::ptree& json)
{
    auto params = extractFrom(json);
    
    if (!params.is_initialized())
    {
        return nullptr;
    } else {
        return T::fromJson(params->second);
    }
}

template <typename S>
boost::property_tree::ptree abstract_instance<S>::toJson() const
{
    boost::property_tree::ptree json;
        
    json.put(PROPERTY_TYPE, S::TYPE);
    json.put_child(PROPERTY_OBJECT, subToJson());
    
    return json;
}


template <typename S>
void abstract_instance<S>::extractCommonParams(boost::property_tree::ptree& json, instance::time_point_t* const time, std::string* const sender)
{
    BOOST_ASSERT(!!sender && !!time);
    // make sure the time and sender are valid
    
    const auto timeIt = json.find(PROPERTY_DATETIME);
    if (timeIt == json.not_found())
    {
        boost::property_tree::write_json(std::cerr, json, true);
        throw std::runtime_error("Could not get datetime property.");
    }
    
    const auto senderIt = json.find(PROPERTY_SENDER);
    if (senderIt == json.not_found())
    {
        boost::property_tree::write_json(std::cerr, json, true);
        throw std::runtime_error("Could not get sender property.");
    }
    
    std::istringstream time_ss(timeIt->second.get_value<std::string>());
    time_ss >> *time;
    
    *sender = json.get<std::string>(PROPERTY_SENDER);
}

template <typename S>
boost::property_tree::ptree abstract_instance<S>::baseJson() const
{

    boost::property_tree::ptree json;
    
    json.put(instance::PROPERTY_DATETIME, this->dateTime());
    json.put(instance::PROPERTY_SENDER, this->sender());
    
    return json;
}


} // end namespace msg

} // end namespace se3313

#endif // SE3313_NETWORK_INSTANCE_HPP_

