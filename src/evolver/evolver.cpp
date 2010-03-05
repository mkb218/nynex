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

void Evolver::initGA(float pMutation, GABoolean elitist, const GAPopulation & pop) {
    delete ga_;
    ga_ = new GASimpleGA(pop);
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
    initGA(pMutation, elitist, pop);
}

void Evolver::stepGA() {
    Ratings::getInstance().getServerRatings();
    BOOST_FOREACH(StepAction& stepaction, prestepactions_) {
        // GVoice download
        // update server ratings
        // check soundcloud dropbox
        stepaction.action(ga_->population());
    }
    ga_->step();
    BOOST_FOREACH(StepAction& stepaction, poststepactions_) {
        // SoundCloud::upload(pop_) + streaming update
        // Twitter notify
        stepaction.action(ga_->population());
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
    float pMut = fromString<float>(strs.front());
    strs.pop_front();
    GABoolean elitist = (fromString<bool>(strs.front()))?gaTrue:gaFalse;
    strs.pop_front();
    size_t expectedPopsize = fromString<size_t>(strs.front());
    strs.pop_front();
    GAPopulation pop;
    BOOST_FOREACH(std::string & s, strs) {
        pop.add(Composition::unserialize(s));
    }
    assert(pop.size() == expectedPopsize);
    initGA(pMut, elitist, pop);
}

void Evolver::saveToFile(const std::string & filename) {
    std::ofstream stream(filename.c_str());
    if (stream.fail()) {
        std::runtime_error("output file open failed for " + filename);
    }
    stream << ga_->pMutation() << TERMINATOR;
    stream << (ga_->elitist()==gaTrue) << TERMINATOR;
    stream << ga_->population().size();
    for (size_t i = 0; i < ga_->population().size(); ++i) {
        Composition & c = static_cast<Composition&>(ga_->population().individual(i));
        stream << TERMINATOR << c.serialize();
    }
    stream.flush();
    stream.close();
}
