#ifndef NYNEX_TWITTER
#define NYNEX_TWITTER

#include <string>
#include <map>

#include "evolver.h"
#include "composition.h"

using namespace boost;

namespace nynex {
    class TwitterServer;
    class TwitterAnnounce : public StepAction {
    public:
        TwitterAnnounce(const TwitterServer & announcer) : announcer_(&announcer){}
        virtual void action(const GAGeneticAlgorithm &);
    private:
        const TwitterServer *announcer_;
    };
    
    class TwitterServer {
    public:
        TwitterServer(const std::string & host, const std::string & twitterKey, const std::string & twitterSecret, const std::string & bitlykeyfile);
        static std::string urlEncode(const std::string & in);
        std::string getBitlyUrl(const std::string & url) const;
        void announceGeneration(const std::string & announcement, const std::string & m3uUrl, const std::string & htmlUrl) const;
    private:
        std::string twitterKey_;
        std::string twitterSecret_;
        std::string bitlylogin_;
        std::string bitlykey_;
    };
    
}
#endif