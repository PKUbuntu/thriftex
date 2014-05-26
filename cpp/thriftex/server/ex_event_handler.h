#ifndef _THRIFTEX_SERVER_EX_EVENT_HANDLER_H_
#define _THRIFTEX_SERVER_EX_EVENT_HANDLER_H_

#include <time.h>
#include <thrift/TProcessor.h>
#include <thrift/server/TServer.h>

namespace thriftex {
namespace server {

using apache::thrift::protocol::TProtocol;
using apache::thrift::transport::TTransport;

/** 
 * Work in connection context
 */
class ExServerEventHandler: public apache::thrift::server::TServerEventHandler
{
public:
    ExServerEventHandler();
    virtual ~ExServerEventHandler();

public:
    /** 
     * Generate inner struct to hold info about one connection
     * 
     * @param input Input protocol
     * @param output Output protocol
     * 
     * @return 
     */
    virtual void* createContext(boost::shared_ptr<TProtocol> input,
                                boost::shared_ptr<TProtocol> output);

    /** 
     * Get remote host / address / port
     * 
     * @param serverContext Generate by #ExServerEventHandler::createContext
     * @param transport Transport underlying
     */
    virtual void processContext(void* serverContext,
                                boost::shared_ptr<TTransport> transport);
  
    /** 
     * Free the inner struct generate by #ExServerEventHandler::createContext
     * 
     * @param serverContext 
     * @param input 
     * @param output 
     */
    virtual void deleteContext(void* serverContext,
                               boost::shared_ptr<TProtocol>input,
                               boost::shared_ptr<TProtocol>output);
};

/** 
 * Work in process(...) context
 */
class ExProcessorEventHandler: public apache::thrift::TProcessorEventHandler
{
public:
    ExProcessorEventHandler();
    virtual ~ExProcessorEventHandler();

public:
    /**
     * Generate inner struct to hold info about one RPC call
     * 
     * @param fn_name Function name of RPC call
     * @param serverContext Generate by ExServerEventHandler
     */
    virtual void* getContext(const char* fn_name, void* serverContext);

    /**
     * Free the struct generate above
     *
     * @param ctx Generate by #ExProcessorEventHandler::getContext
     * @param fn_name Function name of RPC call
     */
    virtual void freeContext(void* ctx, const char* fn_name);
  
    /** 
     * After write back result, print log info
     * 
     * @param ctx Generate by #ExProcessorEventHandler::getContext
     * @param fn_name Function name of RPC call
     * @param bytes The lenght of result
     */
    virtual void postWrite(void* ctx, const char* fn_name, uint32_t bytes);

protected:
    ///< Slow execution
    const static int SLOW_TIME = 100;
};

}}

#endif /* _THRIFTEX_SERVER_EX_EVENT_HANDLER_H_ */
