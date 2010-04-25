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

#include <stdio.h>
#include "evolver.h"
#include "composition.h"

namespace nynex {
class GVoiceAction : public StepAction {
public:
    virtual void action(const GAGeneticAlgorithm & ga);
private:
};
}
#endif