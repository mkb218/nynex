// generic network support thingies
#ifndef NYNEX_NETWORK
#define NYNEX_NETWORK

#include <curl/curl.h>
#include <stdexcept>

namespace nynex {  
    class CurlContext {
    public:
        static volatile CurlContext *getContext();
        ~CurlContext() { curl_global_cleanup(); }
    private:
        CurlContext() { if (0 != curl_global_init(CURL_GLOBAL_NOTHING)) throw std::runtime_error("curl system init failed: "); }
        static volatile CurlContext *context_;
        static volatile bool init_;
    };
    
    void getUrl(const std::string & url);
    void postUrl(const std::string & url, const std::string & body, const std::string & user = "", const std::string & password = "");
}
#endif