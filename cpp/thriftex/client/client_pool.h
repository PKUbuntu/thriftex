#ifndef _THRIFTEX_CLIENT_POOL_H_
#define _THRIFTEX_CLIENT_POOL_H_

#include "thriftex/client/ex_socket.h"
#include "thriftex/client/endpoint.h"

#include "thriftex/base/logging.h"

#include <map>
#include <set>
#include <list>
#include <vector>
#include <iostream>
#include <algorithm>
#include <boost/thread.hpp>
#include <boost/lexical_cast.hpp>
#include <thrift/concurrency/Mutex.h>
#include <thrift/transport/TBufferTransports.h>
#include <thrift/protocol/TBinaryProtocol.h>

namespace thriftex {
namespace client {

using apache::thrift::concurrency::Mutex;
using apache::thrift::concurrency::Guard;

using apache::thrift::transport::TTransport;
using apache::thrift::transport::TFramedTransport;
using apache::thrift::transport::TTransportException;
using apache::thrift::transport::ExSocket;

using apache::thrift::protocol::TProtocol;
using apache::thrift::protocol::TBinaryProtocol;

using boost::shared_ptr;

template <typename ClientType>
class ClientPool
{
public:
    ClientPool(int timeout)
        : timeout_(timeout)
        {}
    virtual ~ClientPool() {}

public:
    virtual ClientType* Get() = 0;

    virtual bool Return(ClientType* client) = 0;

protected:
    ClientType* Make(shared_ptr<Endpoint> endpoint) {
        shared_ptr<ExSocket> socket(new ExSocket(endpoint));

        socket->setConnTimeout(timeout_);
        socket->setSendTimeout(timeout_);
        socket->setRecvTimeout(timeout_);

        socket->setNoDelay(true);
        socket->setLinger(false, 0);
    
        shared_ptr<TTransport> transport(new TFramedTransport(socket));
        shared_ptr<TProtocol> protocol(new TBinaryProtocol(transport));

        ClientType* client = new ClientType(protocol);

        try {
            transport->open();
        } catch (TTransportException& e) {
            EX_ERROR << "thriftexclient: Thrift conn open error addr "
                     << socket->getSocketInfo() << " " << e.what();
            delete client;
            client = NULL;
        } catch (...) {
            EX_ERROR << "thriftex client: Thrift conn open unknown error. addr "
                     << socket->getSocketInfo();
            delete client;
            client = NULL;
        }

        return client;
    }

    /** 
     * Extract inner ExSocket from client
     * 
     * @param client 
     * 
     * @return 
     */
    shared_ptr<ExSocket> GetSocket(ClientType* client) {
        shared_ptr<TProtocol> protocol = client->getInputProtocol();
        shared_ptr<TFramedTransport> framed_transport =
            boost::dynamic_pointer_cast<TFramedTransport,
                                        TTransport>(protocol->getTransport());

        if (!framed_transport) {
            EX_FATAL << "EX: Can not cast pointer from TTransport to TFramedTransport\n";
        }

        shared_ptr<ExSocket> ex_socket =
            boost::dynamic_pointer_cast<ExSocket, TTransport>(framed_transport->getUnderlyingTransport());

        if (!ex_socket) {
            EX_FATAL << "EX: Can not cast pointer from TTransport to ExSocket!\n";
        }

        return ex_socket;
    }
  
    bool Error(ClientType* client) {
        shared_ptr<ExSocket> ex_socket = GetSocket(client);
    
        return ex_socket->get_error();
    }
  
protected:
    int timeout_;
};

/**
 * Simple address parser, to generate Endpoint structure
 *
 * @param addr [in] The address to parse, such as "127.0.0.1:9090"
 * @param host [out] The host, "127.0.0.1"
 * @param port [out] The port, "9090"
 *
 * @return true  The @param addr is valid
 *         false Invalid @param addr
 */
static bool ParseAddr(const std::string& addr, std::string& host,
                      uint16_t& port) {
    size_t pos = addr.find_first_of(':');
    if (pos == std::string::npos) {
        return false;
    }

    host = addr.substr(0, pos);

    try {
        port = boost::lexical_cast<uint16_t>(addr.substr(pos + 1));
    } catch (boost::bad_lexical_cast &) {
        return false;
    }

    return true;
}

}}

#endif // _THRIFTEX_CLIENT_POOL_H_
