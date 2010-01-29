/*
 *  composition.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 1/26/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#include "filemgr.h"

using namespace nynex;

Composition::Composition() : objectId_(nextObjectId_++) {}
Composition::Composition(const std::vector<Word> & words) : objectId_(nextObjectId_++),words_(words.begin(), words.end()) {}
Composition::Composition(const Composition & other) : objectId_(nextObjectId_++),words_(other.words.begin(), othter.words.end()) {}

Composition & Composition::operator=(const GAGenome & other) {
    copy(other);
    return *this;
}

void Composition::copy(const GAGenome & other) {
    Composition & othercast = dynamic_cast<const Composition &>(other);
    words_.assign(othercast.words_.begin(), othercast.words_.end());
}

GAGenome * Composition::clone() const {
    return new Composition(*this);
}

void Composition::init(GAGenome & trg) {
    Composition & trgcast = dynamic_cast<Composition &>(trg);
    SampleBank::getInstance().initComposition(trgcast);
}

int Composition::mutate(GAGenome & trg, float p) {
    Composition & trgcast = dynamic_cast<Composition &>(trg);
    // roll dice on whether to remove sample from beginning
    // roll dice on whether to remove sample from end
    // iterate through words, each time rolling dice to pick a new one
    // roll dice for each note
    // mutate fs1rgen patch
    return count;
}