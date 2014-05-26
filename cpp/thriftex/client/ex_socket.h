#ifndef _THRIFTEX_CLIENT_EX_SOCKET_H_
#define _THRIFTEX_CLIENT_EX_SOCKET_H_

#include "thriftex/client/endpoint.h"

#include <thrift/transport/TSocket.h>
#include <boost/shared_ptr.hpp>

namespace apache { namespace thrift { namespace transport {

/** 
 * When exception occur, the client is marked as error,
 * will not return to client pool.
 */
class ExSocket :public TSocket
{
public:
    ExSocket()
        : error_(false)
        {}

    ExSocket(boost::shared_ptr<thriftex::client::Endpoint> endpoint)
        :TSocket(endpoint->get_host(), endpoint->get_port()),
         error_(false), endpoint_(endpoint)
        {}

    virtual ~ExSocket() {}

    boost::shared_ptr<thriftex::client::Endpoint> get_endpoint() {
        return endpoint_;
    }
  
    ///< override
    virtual uint32_t read_virt(uint8_t *buf, uint32_t len) {
        try {
            return read(buf, len);
        } catch (TTransportException &tex) {
            set_error(true);
            throw tex;
        }
    }

    ///< override
    virtual uint32_t readAll_virt(uint8_t *buf, uint32_t len) {
        try {
            return readAll(buf, len);
        } catch (TTransportException &tex) {
            set_error(true);
            throw tex;
        }
    }

    ///< override
    virtual void write_virt(const uint8_t *buf, uint32_t len) {
        try {
            write(buf, len);
        } catch (TTransportException &tex) {
            set_error(true);
            throw tex;
        }
    }

    ///< override
    virtual const uint8_t* borrow_virt(uint8_t *buf, uint32_t* len) {
        try {
            return borrow(buf, len);
        } catch (TTransportException &tex) {
            set_error(true);
            throw tex;
        }
    }
  
    ///< override
    virtual void consume_virt(uint32_t len) {
        try {
            return consume(len);
        } catch (TTransportException &tex) {
            set_error(true);
            throw tex;
        }
    }
  
    void set_error(bool error) {
        error_ = error;
    }

    bool get_error() {
        return error_;
    }

protected:
    bool error_;
    boost::shared_ptr<thriftex::client::Endpoint> endpoint_;
};

}}}

#endif
