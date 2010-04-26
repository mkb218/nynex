/*
 *  uploads.h
 *  nynex
 *
 *  Created by Matthew J Kane on 4/18/10.
 *  Copyright 2010 Matt Kane. All rights reserved.
 *
 */

#ifndef NYNEX_UPLOADS
#define NYNEX_UPLOADS

#include "evolver.h"
#include "composition.h"

namespace nynex {
    class GrabUploadsAction : public StepAction {
    public:
        GrabUploadsAction(const std::string & user, const std::string & server, const std::string & path) : server_(server), path_(path), user_(user) {}
        virtual void action(const GAGeneticAlgorithm &);
    private:
        std::string server_;
        std::string user_;
        std::string path_;
    };
}

#endif