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
#include "composition.h"

#include <stdexcept>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#define TERMINATOR ';'
#ifndef DELIMITER
#define DELIMITER ':'
#endif
#if (TERMINATOR == DELIMITER)
#error need different terminator and delimiter characters!
#endif

using namespace nynex;

Evolver::Evolver() {
    ga_ = NULL;
}

void Evolver::initGA(float pMutation, GABoolean elitist, int gen, const GAPopulation & pop) {
    delete ga_;
    ga_ = new NynexGA(pop, gen);
    ga_->elitist(elitist);
    ga_->pMutation(pMutation);
    std::cout << ga_->pCrossover() << std::endl;
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
    initGA(pMutation, elitist, 0, pop);
}

void Evolver::stepGA() {
    Ratings::getInstance().getServerRatings();
    BOOST_FOREACH(StepAction * stepaction, prestepactions_) {
        // GVoice download
        // update server ratings
        // check soundcloud dropbox
        stepaction->action(*ga_);
    }
    ga_->step();
    BOOST_FOREACH(StepAction * stepaction, poststepactions_) {
        // SoundCloud::upload(pop_) + streaming update
        // Twitter notify
        stepaction->action(*ga_);
    }
}

const GAPopulation & Evolver::getPop() {
    if (ga_ == NULL) { throw std::logic_error("getPop called with null GA"); }
    return ga_->population();
}

const GAGeneticAlgorithm & Evolver::getGA() {
    if (ga_ == NULL) { throw std::logic_error("getGA called with null GA"); }
    return *ga_;
}

void Evolver::loadFromFile(const std::string & filename) {
    std::ifstream stream(filename.c_str());
    if (stream.fail()) {
        std::runtime_error("input file failed for " + filename);
    }
    std::string serialized;
    stream >> serialized;
    std::list<std::string> strs;
    boost::split(strs, serialized, boost::is_from_range(TERMINATOR, TERMINATOR));
    if (strs.size() < 5) { 
        throw std::runtime_error("not enough elements in strs");
    }
    float pMut = fromString<float>(strs.front());
    strs.pop_front();
    GABoolean elitist = (fromString<bool>(strs.front()))?gaTrue:gaFalse;
    strs.pop_front();
    int gen = fromString<int>(strs.front());
    strs.pop_front();
    size_t expectedPopsize = fromString<size_t>(strs.front());
    strs.pop_front();
    GAPopulation pop;
    BOOST_FOREACH(std::string & s, strs) {
        pop.add(Composition::unserialize(s));
    }
    if(pop.size() != expectedPopsize) {
        throw std::runtime_error("popsize " + stringFrom(pop.size()) + " != " + stringFrom(expectedPopsize));
    }
    initGA(pMut, elitist, gen, pop);
}

void Evolver::saveToFile(const std::string & filename) {
    std::ofstream stream(filename.c_str());
    if (stream.fail()) {
        std::runtime_error("output file open failed for " + filename);
    }
    stream << ga_->pMutation() << TERMINATOR;
    stream << (ga_->elitist()==gaTrue) << TERMINATOR;
    stream << (ga_->generation()) << TERMINATOR;
    stream << ga_->population().size();
    for (size_t i = 0; i < ga_->population().size(); ++i) {
        Composition & c = static_cast<Composition&>(ga_->population().individual(i));
        stream << TERMINATOR << c.serialize();
    }
    stream.flush();
    stream.close();
}

void Evolver::addNotifier(bool pre, StepAction *s) {
    std::list<StepAction*> & actions = (pre)?prestepactions_:poststepactions_;
}

Evolver::~Evolver() {
    delete ga_;
    BOOST_FOREACH(StepAction * s, prestepactions_) {
        delete s;
    }
    BOOST_FOREACH(StepAction * s, poststepactions_) {
        delete s;
    }
}