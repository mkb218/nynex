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
        void fetchDropBox(); // uses samplebank singleton
        std::vector<std::string> submitCompositions(const std::vector<boost::reference_wrapper<Composition> > &) const; // returns list of identifiers used for streaming
    private:
        std::string host_;
        std::string apiKey_;
    };
}

#endif