/*
 *  gvoice.h
 *  nynex
 *
 *  Created by Matthew J Kane on 4/14/10.
 *  Copyright 2010 Matt Kane. All rights reserved.
 *
 */

#ifndef NYNEX_GVOICE
#define NYNEX_GVOICE

class GVoiceAction : public StepAction {
public:
    GVoiceAction(const std::string & scriptpath) : scriptpath_(scriptpath) {}
    virtual void action(const GAGeneticAlgorithm & ga) {
        system((std::string("python ") + scriptpath_ + " gv.py").c_str());
    }
private:
    std::string scriptpath_;
};

#endif