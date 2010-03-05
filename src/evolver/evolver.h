/*
 *  evolver.h
 *  nynex
 *
 *  Created by Matthew J Kane on 3/3/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#ifndef NYNEX_EVOLVER
#define NYNEX_EVOLVER

#include "composition.h"
#include <boost/ref.hpp>

namespace nynex {
    class StepAction {
    public:
        virtual void action(const GAPopulation &) = 0;
    };
    
    class Evolver {
    public:
        Evolver();
        void initGA(float pMutation, int pop, GABoolean elitist);
        void stepGA(); // apply ratings and step
        const GAPopulation & getPop();
        const GAGeneticAlgorithm & getGA();
        void loadFromFile(const std::string & filename);
        void saveToFile(const std::string & filename);
        void addNotifier(bool pre);
    private:
        GASimpleGA *ga_;
        std::list<boost::reference_wrapper<StepAction> > prestepactions_;
        std::list<boost::reference_wrapper<StepAction> > poststepactions_;
    };
}

#endif