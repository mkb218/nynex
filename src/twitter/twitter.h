#ifndef NYNEX_TWITTER
#define NYNEX_TWITTER

#include <string>
#include <map>
#include <boost/ref.hpp>

using namespace boost;

namespace nynex {
    class UrlShortener {
    public:
        static UrlShortener *getShortener(const std::string & provider);
        static void setConfig(const std::string & filename);
        virtual std::string shortenUrl(std::string & longUrl) const = 0;
    private:
        static std::map<std::string, reference_wrapper<const UrlShortener> > instances_;
    };
    
    class TwitterServer {
    public:
        void announceGeneration(const std::string & listenUrl, const std::string & pageUrl);
    private:
        std::string host_;
        std::string apiKey_;
        UrlShortener *shortener_;
    };
    
}
#endif