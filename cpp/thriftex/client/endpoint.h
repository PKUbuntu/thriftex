#ifndef _THRIFTEX_CLIENT_ENDPOINT_H_
#define _THRIFTEX_CLIENT_ENDPOINT_H_

#include <stdint.h>
#include <string>
#include <map>
#include <ostream>
#include <functional>
#include <list>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/lexical_cast.hpp>

namespace thriftex {
namespace client {

/** 
 * The basic structure to identify one service in client's view.
 *
 * Some infomation here:
 *  1. Address, host:port
 *  2. The status of the service, such as enable/disable, fail
 *
 * NOTE: The load of one endpoint, you can use shared_ptr::use_count()
 */
class Endpoint :public boost::noncopyable {
public:
    Endpoint()
        : host_(""), port_(0), ctime_(0), mtime_(0),
          failures_(0), enabled_(false), healthy_(true)
        {}

    Endpoint(const std::string &host, uint16_t port)
        : host_(host), port_(port), ctime_(0), mtime_(0),
          failures_(0), enabled_(false), healthy_(true)
        {}

    ~Endpoint() {}

    void set_host(const std::string &host) {
        host_ = host;
    }
  
    std::string get_host() const {
        return host_;
    }

    void set_port(uint16_t port) {
        port_ = port;
    }

    uint16_t get_port() const {
        return port_;
    }

    void set_ctime(int64_t ctime) {
        ctime_ = ctime;
    }

    int64_t get_ctime() {
        return ctime_;
    }
  
    void set_mtime(int64_t mtime) {
        mtime_ = mtime;
    }
  
    int64_t get_mtime() {
        return mtime_;
    }
  
    void set_enable(bool enable) {
        enabled_ = enable;
    }

    bool get_enabled() const {
        return enabled_;
    }

    void set_healthy(bool healthy) {
        healthy_ = healthy;
    }
  
    bool get_healthy() const {
        return healthy_;
    }

    /** 
     * Get the node name (zookeeper node file name)
     *
     *   format: "host:port"
     * 
     * @return node name on zookeeper
     */
    std::string GetNodeName() {
        std::string node_name = host_;
        node_name += ":";
        node_name += boost::lexical_cast<std::string>(port_);
        return node_name;
    }
  
    /** 
     * Whether the endpoint is ready to use
     * 
     * @return 
     */
    bool Active() const {
        return enabled_ && healthy_;
    }

    /** 
     * Use host:port + ctime(create time) to compare
     * 
     * @param lhs 
     * @param rhs 
     * 
     * @return true if node name and ctime are both same
     */
    friend bool operator==(const Endpoint &lhs, const Endpoint &rhs) {
        return lhs.host_ == rhs.host_ && lhs.port_ == rhs.port_ &&
            lhs.ctime_ == rhs.ctime_;
    }

    friend bool operator<(const Endpoint &lhs, const Endpoint &rhs) {
        return (lhs.host_ < rhs.host_ ||
                (lhs.host_ == rhs.host_ && lhs.port_ < rhs.port_) ||
                (lhs.host_ == rhs.host_ && lhs.port_ == rhs.port_ && lhs.ctime_ < rhs.ctime_));
    }
  
    friend std::ostream& operator<<(std::ostream &os, const Endpoint &endpoint) {
        os << endpoint.host_ << ":" << endpoint.port_
           << " ctime:" << endpoint.ctime_
           << " mtime:" << endpoint.mtime_
           << " enable:" << endpoint.enabled_
           << " healthy:" << endpoint.healthy_;

        return os;
    }

private:
    std::string host_;
    uint16_t port_;

    int64_t ctime_;                       ///< Create time of znode
    int64_t mtime_;                       ///< Last modity time of znode
  
    int failures_;                        ///< Number of failures of this endpoint
  
    bool enabled_;
    bool healthy_;
};

typedef std::list<boost::shared_ptr<Endpoint> > EndpointListType;

}}

namespace std {

// Some basic functors, so you can use stl algorithm such as:
//  sort, find_if and so on.
template <>
struct equal_to<boost::shared_ptr<thriftex::client::Endpoint> >
    : public binary_function<boost::shared_ptr<thriftex::client::Endpoint>,
                             boost::shared_ptr<thriftex::client::Endpoint>, bool>
{
    bool operator()(const boost::shared_ptr<thriftex::client::Endpoint>& __x,
                    const boost::shared_ptr<thriftex::client::Endpoint>& __y) const
        {
            return *__x == *__y;
        }
};


template <>
struct less<boost::shared_ptr<thriftex::client::Endpoint> >
    : public binary_function<boost::shared_ptr<thriftex::client::Endpoint>,
                             boost::shared_ptr<thriftex::client::Endpoint>, bool>
{
    bool operator()(const boost::shared_ptr<thriftex::client::Endpoint>& __x,
                    const boost::shared_ptr<thriftex::client::Endpoint>& __y) const
        {
            return *__x < *__y;
        }
};

template <>
struct greater_equal<boost::shared_ptr<thriftex::client::Endpoint> >
    : public binary_function<boost::shared_ptr<thriftex::client::Endpoint>,
                             boost::shared_ptr<thriftex::client::Endpoint>, bool>
{
    bool operator()(const boost::shared_ptr<thriftex::client::Endpoint>& __x,
                    const boost::shared_ptr<thriftex::client::Endpoint>& __y) const
        {
            return !(*__x < *__y);
        }
};


}
#endif
