// generic network support thingies
#ifndef NYNEX_NETWORK
#define NYNEX_NETWORK

#include <CFHTTPMessage.h>
#include <list>

namespace nynex {
    class HttpPool {
    public:
        ~HttpPool() { deleteRefs(); }
        HttpPool & getActivePool() { return *activePool_; }
        static void setActivePool(HttpPool * pool) { activePool_ = pool; }
        void addRef(CFTypeRef r) { refs_.push_back(r); }
        void deleteRefs() { while (!refs_.empty()) { CFRelease(refs.front()); refs.pop_front(); }
    private:
        static HttpPool *activePool_;
        std::list<CFTypeRef> refs_;
    };
    
    CFHTTPAuthenticationRef getAuthRef(CFHTTPMessageRef errResponse, const std::string & username, const std::string & password);
    CFHTTPMessageRef getUrl(const std::string & url, CFHTTPAuthenticationRef auth = NULL);
    CFHTTPMessageRef postUrl(const std::string & url, const std::string & body, CFHTTPAuthenticationRef auth = NULL);

#endif