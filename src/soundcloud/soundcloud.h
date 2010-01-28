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
        void submitCompositions(const std::vector<boost::reference_wrapper<Composition> > &) const;
    private:
        std::string host;
        std::string apiKey;
    };

#endif