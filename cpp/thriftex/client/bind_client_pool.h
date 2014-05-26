#ifndef _THRIFTEX_CLIENT_BIND_CLIENT_POOL_H_
#define _THRIFTEX_CLIENT_BIND_CLIENT_POOL_H_

#include "thriftex/client/client_pool.h"

namespace thriftex {
namespace client {

/** 
 * Client pool used when binding address
 */
template <typename ClientType>
class BindClientPool: public ClientPool<ClientType>
{
public:
    BindClientPool(const std::string& host, uint16_t port, int timeout)
        : ClientPool<ClientType>(timeout), endpoint_(new Endpoint(host, port)),
          max_size_(FLAGS_client_pool_max)
        {}

    virtual ~BindClientPool() {
        typename std::list<ClientType *>::iterator it = client_list_.begin();
        for (; it != client_list_.end(); ++it) {
            delete *it;
        }    
    }

public:
    virtual ClientType* Get() {
        Guard g(mutex_);
        ClientType *client = NULL;

        if (client_list_.size() > 0) {
            client = client_list_.front();
            client_list_.pop_front();
        } else if (endpoint_.use_count() - 1 < max_size_) {
            client = this->Make(endpoint_);
            EX_DEBUG_INFO << "Client allocate, current number of client in use ["
                          << endpoint_.use_count() - 1 << "]";
        }
        return client;
    }

    virtual bool Return(ClientType *client) {
        Guard g(mutex_);
        if (client != NULL) {
            if (!this->Error(client)) {
                client_list_.push_back(client);
            } else {
                EX_WARN << "Exception occur, refused to return client ["
                        << *(this->GetSocket(client)->get_endpoint()) << "]!";
                delete client;
                return false;
            }
        } else {
            EX_ERROR << "Return NULL, some error occurs!";
            return false;
        }
        return true;
    }

protected:
    Mutex mutex_;
    shared_ptr<Endpoint> endpoint_;
    std::list<ClientType *> client_list_;
    // max client can hold in bind mode
    int max_size_;
};

}}

#endif 
