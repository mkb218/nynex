#ifndef NYNEX_TWITTER
#define NYNEX_TWITTER

#include <string>
#include <map>

#include "evolver.h"
#include "composition.h"

using namespace boost;

namespace nynex {
    class TwitterAnnounce : public StepAction {
    public:
        TwitterAnnounce(const TwitterAnnounce & announcer);
        virtual void action(const GAGeneticAlgorithm &);
    };
    
    class TwitterServer {
    public:
        TwitterServer(const std::string & host, const std::string & username, const std::string & password, const std::string & bitlykeyfile);
        static std::string urlEncode(const std::string & in);
        std::string getBitlyUrl(const std::string & url) const;
        void announceGeneration(const std::string & announcement, const std::string & m3uUrl, const std::string & htmlUrl) const;
    private:
        std::string username_;
        std::string password_;
        std::string bitlykey_;
    };
    
}
#endif