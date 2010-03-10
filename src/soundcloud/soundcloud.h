/*
 *  soundcloud.h
 *  nynex
 *
 *  Created by Matthew J Kane on 1/28/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#ifndef NYNEX_SOUNDCLOUD
#define NYNEX_SOUNDCLOUD

#include <boost/ref.hpp>

#include "SoundCloudCAPI.h" // this header has extern "C" 
#include "composition.h"
#include "evolver.h"

namespace nynex {
    class SoundCloudServer;
    
    class SubmitSoundCloudAction : public StepAction {
    public:
        SubmitSoundCloudAction(const SoundCloudServer & server) : server_(&server) {}
        virtual void action(const GAPopulation &);
    private:
        const SoundCloudServer * server_;
    };
    
    class SoundCloudDropBoxAction : public StepAction {
    public:
        SoundCloudDropBoxAction(const SoundCloudServer & server) : server_(&server) {}
        virtual void action(const GAPopulation &);
    private:
        const SoundCloudServer * server_;
    };
    
    class SoundCloudAuthenticator {
    public:
        virtual ~SoundCloudAuthenticator();
        virtual bool authenticate() = 0;
    protected:
        SoundCloudAuthenticator(SoundCloudCAPI * api) : api_(api) {}
        bool authserver();
        void setverifier(const std::string & buffer);
    private:
        SoundCloudCAPI * api_;
    };
    
    class TerminalAuthenticator : public SoundCloudAuthenticator {
    public:
        TerminalAuthenticator(SoundCloudCAPI * api) : SoundCloudAuthenticator(api){}
        static SoundCloudAuthenticator* create(SoundCloudCAPI*);
        virtual bool authenticate();
    };
    
//    typedef scauthptr;
    typedef SoundCloudAuthenticator *(*SoundCloudAuthGenerator)(SoundCloudCAPI*);
    
    class SoundCloudServer {
    public:
        static SoundCloudServer StagingServerFactory(std::string host, std::string username, std::string password, const std::string & filename) { return SoundCloudServer(host, username, password, filename); }
        static SoundCloudServer ProdServerFactory(std::string host, std::string apiKey, const std::string & filename) { return SoundCloudServer(host, apiKey, filename); }
        static void setDefaultAuthenticatorGenerator(SoundCloudAuthGenerator a) { defaultAuthenticatorGenerator_ = a; }
        void fetchDropBox() const; // uses samplebank singleton and afconvert to add samples
        std::vector<std::string> submitCompositions(const std::vector<boost::reference_wrapper<Composition> > &) const; // returns list of identifiers used for streaming
    private:
        SoundCloudServer(std::string host, std::string username, std::string password, const std::string & filename) : host_(host), username_(username), password_(password), useKey_(false), authenticated_(false), scApi_(NULL) { init(filename); }
        SoundCloudServer(std::string host, std::string apiKey, const std::string & filename) : host_(host), apiKey_(apiKey), useKey_(true), authenticated_(false) { init(filename); }
        void init(const std::string & filename) { initSecrets(filename); scApi_ = SoundCloudCAPI_CreateWithDefaultCallbackAndGetCredentials(consumerKey_.c_str(), consumerSecret_.c_str(), "", 1); }
        void initSecrets(const std::string & filename);
        void authenticate();
        static SoundCloudAuthGenerator defaultAuthenticatorGenerator_;
        SoundCloudAuthenticator * authenticator_;
        SoundCloudCAPI *scApi_;
        std::string host_;
        std::string username_;
        std::string password_;
        std::string apiKey_;
        std::string consumerKey_;
        std::string consumerSecret_;
        bool useKey_;
        bool authenticated_;
    }; 
}

#endif