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

#include "composition.h"

namespace nynex {
    class SoundCloudServer {
    public:
        static SoundCloudServer StagingServerFactory(std::string host, std::string username, std::string password) { return SoundCloudServer(host, username, password); }
        static SoundCloudServer ProdServerFactory(std::string host, std::string apiKey) { return SoundCloudServer(host, apiKey); }
        void fetchDropBox(); // uses samplebank singleton and afconvert to add samples
        std::vector<std::string> submitCompositions(const std::vector<boost::reference_wrapper<Composition> > &) const; // returns list of identifiers used for streaming
    private:
        SoundCloudServer(std::string host, std::string username, std::string password) : host_(host), username_(username), password_(password), useKey_(false) {}
        SoundCloudServer(std::string host, std::string apiKey) : host_(host), apiKey_(apiKey), useKey_(true) {}
        std::string host_;
        std::string username_;
        std::string password_;
        std::string apiKey_;
        bool useKey_;
    }; 
}

#endif