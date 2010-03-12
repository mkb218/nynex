// generic network support thingies
#ifndef NYNEX_NETWORK
#define NYNEX_NETWORK

#include <list>

namespace nynex {  
    class CurlContext {
    public:
        static CurlContext *getContext() { if (context_ == NULL) context = new CurlContext(); return context_; }
        ~CurlContext() { curl_global_cleanup(); }
    private:
        CurlContext() { curl_global_init(); }
        static CurlContext *context_;
    };
    
    void getUrl(const std::string & url);
    void postUrl(const std::string & url, const std::string & body, CFHTTPAuthenticationRef auth = NULL);
}
#endif