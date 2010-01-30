/*
 *  composition.cpp
 *  nynex
 *  $Id$
 *
 *  Created by Matthew J Kane on 1/26/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#include "composition.h"
#include <cstdlib>
#include <map>

using namespace nynex;

Composition::Composition() : objectId_(nextObjectId_++) {}
Composition::Composition(const std::list<Word> & words) : objectId_(nextObjectId_++),words_(words.begin(), words.end()) {}
Composition::Composition(const Composition & other) : objectId_(other.objectId_),words_(other.words_.begin(), other.words_.end()) {}

Composition & Composition::operator=(const GAGenome & other) {
    copy(other);
    return *this;
}

void Composition::copy(const GAGenome & other) {
    const Composition & othercast = dynamic_cast<const Composition &>(other);
    words_.assign(othercast.words_.begin(), othercast.words_.end());
}

GAGenome * Composition::clone() const {
    // clone is meant to be a distinct entity
    return new Composition(words_);
}

void Composition::init(GAGenome & trg) {
    Composition & trgcast = dynamic_cast<Composition &>(trg);
    SampleBank::getInstance().initComposition(trgcast);
}

int Composition::mutate(GAGenome & trg, float p) {
    Composition & trgcast = dynamic_cast<Composition &>(trg);
    // roll dice on whether to remove sample from beginning
    // random float < p means remove first sample
    // roll dice on whether to remove sample from end
    // random float < p means remove end sample
    // iterate through words, each time rolling dice to pick a new one
    // random float < p means choose a random new sample to replace it
    // roll dice for remove a note
    // random float < p means remove first note in map, whatever it is
    // roll dice for add a note iff notes_.size < 4
    // random float < p means choose new note in map
    // roll dice for each note
    // random float < p means replace with new note
    // mutate fs1rgen patch
    return count;
}

float Composition::compare(const GAGenome & left, const GAGenome & right) {
    float diffs = 0;
    float denominator = 0;
    Composition & leftcast = dynamic_cast<Composition &>(left);
    Composition & rightcast = dynamic_cast<Composition &>(right); 
    std::list<Word>::const_iterator leftit = leftcast.words_.begin();
    std::list<Word>::const_iterator rightit = rightcast.words_.begin();
    while (leftit != leftcast.words_.end() && rightit != rightcast.words_.end()) {
        if (leftit->getFilename() != rightit->getFilename()) {
            diffs++;
        }
        denominator++;
    }
    
    int sizediff = abs(leftcast.words_.size() - rightcast.words_.size());
    diffs += sizediff;
    denominator += sizediff;
 
/*
    denominator += leftcast.notes_.size() + rightcast.notes_.size();
    std::map<char> notes;
    notes.insert(leftcast.notes_.begin(), leftcast.notes_.end());
    for (std::map<char>::const_iterator it = rightcast.notes_.begin(); ++it; it != rightcast.notes_.end()) {
        notes_.erase(*it);
    }
    diffs += notes.size();
 
    diffs += comparison of fsr1gen models
    denominator += fs1rgen model size
 */
    return diffs/denominator;
}

int Composition::crossover(const GAGenome & mom, const GAGenome & dad, GAGenome * bro, GAGenome * sis) {
    // choose crossover point <= min(mom.words_.size(), dad.words_.size()) and >= 0
    // mom's words to left of crossover point go to beginning of bro
    // dad's words from right of crossover point go to end of bro
    // dad's words from left of crossover point go to beginning of sis
    // mom's words from right of crossover point go to end of sis
    // crossover fs1rgen
    // choose crossover point <= min(mom.notes_.size(), dad.notes_.size()) and >= 0
    // same pattern as for words
}

void Composition::bounceToFile(const std::string & filename) const {
    // concatenate all words to libsox to filename with format autodetect
    // calculate length of sample
    // hold notes for length of sample while recording on audio in
}

Word::Word(const std::string & filename, int age) : filename_(filename), age_(age), score_(0.0) {
    calcDuration();
}

std::string & Word::getFilename() const {
    return filename_;
}

int Word::getAge() const {
    return age_;
}

float Word::getScore() const {
    return score_;
}

void Word::setScore(float score) {
    score_ = score;
}

unsigned int Word::getDuration() const {
    return duration_;
}

void Word::calcDuration() {
    // ask libsox for duration of filename
}

Sample::Sample(const std::string & filename) : filename_(filename) {
}