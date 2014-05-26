#include "thriftex/server/ex_event_handler.h"
#include "thriftex/base/logging.h"

#include <thrift/transport/TSocket.h>
#include <thrift/transport/TBufferTransports.h>

#include <sys/time.h>
#include <iostream>

namespace thriftex {
namespace server {

using apache::thrift::transport::TSocket;
using apache::thrift::transport::TMemoryBuffer;
using apache::thrift::transport::TFramedTransport;

/** 
 * Connection info
 */
struct ServerContext {
    std::string remote_host;
    std::string remote_address;
    int port;
};

/** 
 * Process info
 */
struct ProcessContext {
    ServerContext *server_context;
    ::timeval timeval;
};

ExServerEventHandler::ExServerEventHandler()
{}

ExServerEventHandler::~ExServerEventHandler()
{}

void *
ExServerEventHandler::createContext(boost::shared_ptr<TProtocol>,
                                     boost::shared_ptr<TProtocol>) {
    ServerContext *context = new ServerContext();
    return context;
}

void
ExServerEventHandler::processContext(void* serverContext,
                                      boost::shared_ptr<TTransport> transport) {
    ServerContext *context = static_cast<ServerContext *>(serverContext);
    TSocket *tSocket = static_cast<TSocket *>(transport.get());

    if (context != NULL && tSocket != NULL) {
        context->remote_host = tSocket->getPeerHost();
        context->remote_address = tSocket->getPeerAddress();
        context->port = tSocket->getPeerPort();
    } else {
        EX_ERROR << "Invalid server context or tSocket!\n";
    }
}


void
ExServerEventHandler::deleteContext(void* serverContext,
                                     boost::shared_ptr<TProtocol>,
                                     boost::shared_ptr<TProtocol>) {
    ServerContext *context = static_cast<ServerContext *>(serverContext);
    delete context;
}


ExProcessorEventHandler::ExProcessorEventHandler()
{}

ExProcessorEventHandler::~ExProcessorEventHandler()
{}

void *
ExProcessorEventHandler::getContext(const char *, void *serverContext) {
    ProcessContext *process_context = new ProcessContext();

    if (gettimeofday(&process_context->timeval , NULL) != 0) {
        delete process_context;
        process_context = NULL;
    } else {
        process_context->server_context = static_cast<ServerContext *>(serverContext);
    }

    return process_context;
}

void
ExProcessorEventHandler::freeContext(void *ctx, const char *) {
    delete static_cast<ProcessContext *>(ctx);
    ctx = NULL;
}

void
ExProcessorEventHandler::postWrite(void *ctx, const char *fn_name, uint32_t) {
    ::timeval timeval_current;
    ::timeval &timeval_before_ptr =
          static_cast<ProcessContext *>(ctx)->timeval;

    ServerContext *server_context =
        static_cast<ProcessContext *>(ctx)->server_context;
  
    if (gettimeofday(&timeval_current, NULL) == 0) {
        int ms = (timeval_current.tv_sec - timeval_before_ptr.tv_sec) * 1000;
        ms += (timeval_current.tv_usec - timeval_before_ptr.tv_usec) / 1000;

        if (ms >= this->SLOW_TIME) {
            EX_WARN << server_context->remote_host << "("
                     << server_context->remote_address << ":" << server_context->port
                     << ") " << fn_name << ".SLOW " << ms << "ms cost\n";
        } else {
            EX_DEBUG_INFO << server_context->remote_host << "("
                           << server_context->remote_address << ":"
                           << server_context->port << ") " << fn_name << " "
                           << ms << "ms cost\n";
        }
    }
}

}}
