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
    std::string fileForGenAndIndividual(int gen, int i);
    
    class SoundCloudAuthenticator {
    public:
        virtual ~SoundCloudAuthenticator() {}
        virtual bool authenticate() = 0;
    protected:
        SoundCloudAuthenticator(SoundCloudCAPI * api) : api_(api) {}
        int authserver();
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

    class FileAuthenticator : public SoundCloudAuthenticator {
    public:
        FileAuthenticator(SoundCloudCAPI * api) : SoundCloudAuthenticator(api){}
        static SoundCloudAuthenticator* create(SoundCloudCAPI*);
        virtual bool authenticate();
    };
    
    typedef SoundCloudAuthenticator *(*SoundCloudAuthGenerator)(SoundCloudCAPI*);
    
    class SoundCloudServer {
    public:
        SoundCloudServer(const std::string & key, const std::string & secret, const std::string & filepath, const std::string & scpcmd, bool useSandbox);
        ~SoundCloudServer();
        std::vector<std::string> submitCompositions(const GAGeneticAlgorithm &) const; // returns list of identifiers used for streaming / web ratings
        const std::string & getFilepath() const { return filepath_; }
        const std::string & getScpcmd() const { return scpcmd_; }
        void setFilepath(const std::string & s) { filepath_ = s; }
        void setScpcmd(const std::string & s) { scpcmd_ = s; }
    private:
        SoundCloudServer(const SoundCloudServer &);
        SoundCloudServer & operator=(const SoundCloudServer &);
        void authenticate() const;
        static SoundCloudAuthGenerator defaultAuthenticatorGenerator_;
        mutable SoundCloudAuthenticator * authenticator_;
        SoundCloudCAPI *scApi_;
        bool useSandbox_;
        mutable bool authenticated_;
        std::string key_;
        std::string secret_;
        std::string scpcmd_;
        std::string filepath_;
    }; 

    class SubmitSoundCloudAction : public StepAction {
    public:
        SubmitSoundCloudAction(const SoundCloudServer & server) : server_(&server) {}
        virtual void action(const GAGeneticAlgorithm & ga);
    private:
        const SoundCloudServer * server_;
    };
    
    class SoundCloudDropBoxAction : public StepAction {
    public:
        SoundCloudDropBoxAction(const SoundCloudServer & server) : server_(&server) {}
        virtual void action(const GAGeneticAlgorithm & ga);
    private:
        const SoundCloudServer * server_;
    };
    
    
}

#endif