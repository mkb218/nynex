/*
 *  evolver.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 3/3/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#include "evolver.h"
#include "ratings.h"

#include <stdexcept>
using namespace nynex;

Evolver::Evolver() {
    ga_ = NULL;
}
    
void Evolver::initGA(float pMutation, int popSize, GABoolean elitist) {
    GAPopulation pop;
    for (; popSize > 0; --popSize) {
        try {
            Composition *c = new Composition();
            SampleBank::getInstance().initComposition(*c);
            pop.add(c);
        } catch (std::bad_alloc & e) {
            throw std::runtime_error("couldn't alloc new composition");
        }
    }
    delete ga_;
    ga_ = new GASimpleGA(pop);
    ga_->elitist(elitist);
    ga_->pMutation(pMutation);
    std::cout << ga_->pCrossover() << std::endl;
}

void Evolver::stepGA() {
    Ratings::getInstance().getServerRatings();
    ga_->step();
    // foreach stepaction in stepactions_
    // stepaction->action
    // SoundCloud::upload(pop_) + streaming update + check dropbox
    // GVoice download
    // Twitter notify
    // update server ratings
}

const GAPopulation & Evolver::getPop() {
    if (ga_ == NULL) { throw std::logic_error("getPop called with null GA"); }
    return ga_->population();
}

const GAGeneticAlgorithm & Evolver::getGA() {
    if (ga_ == NULL) { throw std::logic_error("getGA called with null GA"); }
    return *ga_;
}