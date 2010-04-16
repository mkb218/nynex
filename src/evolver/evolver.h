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

namespace nynex {
    GABoolean neverTerminate(GAGeneticAlgorithm &);
    
    class StepAction {
    public:
        virtual void action(const GAGeneticAlgorithm &) = 0;
        virtual ~StepAction() {}
    };
    
    class NynexGAStats : public GAStatistics {
    public:
        NynexGAStats() : GAStatistics() {}
        NynexGAStats(const GAStatistics & other) : GAStatistics(other) {}
        NynexGAStats(const NynexGAStats & other) : GAStatistics(other) {}
        void setGen(int gen) { curgen = gen; }
    };
    
    class NynexGA : public GASimpleGA {
    public:
        NynexGA(const GAPopulation & pop, int genStart) : GASimpleGA(pop) {
            NynexGAStats s(stats);
            s.setGen(genStart);
            stats = s;
        }
    };      
    
    class Evolver {
    public:
        Evolver();
        ~Evolver();
        void initGA(float pMutation, int pop, GABoolean elitist);
        void stepGA(); // apply ratings and step
        const GAPopulation & getPop();
        const GAGeneticAlgorithm & getGA();
        void loadFromFile(const std::string & filename);
        void saveToFile(const std::string & filename);
        void addNotifier(bool pre, StepAction *);
    private:
        void initGA(float pMutation, GABoolean elitist, int gen, const GAPopulation &);
        NynexGA *ga_;
        std::list<StepAction*> prestepactions_;
        std::list<StepAction*> poststepactions_;
    };
}

#endif