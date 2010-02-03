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
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <cmath>

using namespace nynex;
using std::sort;

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
    int count = 0;
    SampleBank & samplebank = SampleBank::getInstance();
    
    // roll dice on whether to remove sample from beginning
    // random float < p means remove first sample
    if (random() % 10000 / 10000.0 > p) {
        words_.pop_front();
    }
    
    // roll dice on whether to remove sample from end
    // random float < p means remove end sample
    if (random() % 10000 / 10000.0 > p) {
        words_.pop_back();
    }
    
    // iterate through words, each time rolling dice to pick a new one
    // random float < p means choose a random new sample to replace it
    for (std::list<Word>::iterator it = words_.begin(); it != words_.end(); ++it) {
        if (random() % 10000 / 10000.0 > p) {
            *it = samplebank.randomWord();
        }
    }
    
    // below needs fs1rgen integration
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
    const Composition & leftcast = dynamic_cast<const Composition &>(left);
    const Composition & rightcast = dynamic_cast<const Composition &>(right); 
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

const std::string & Word::getFilename() const {
    return filename_;
}

bool Word::operator<(const Word & other) const {
    return ( age_ < other.age_ );
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

SampleBank* SampleBank::instance_ = NULL;

SampleBank & SampleBank::getInstance() {
    if (instance_ == NULL) {
        instance_ = new SampleBank();
    }
    return *instance_;
}

SampleBank::SampleBank() {
    srandomdev();
    needsResort_ = false;
}

void SampleBank::setSampleDir(const std::string & dir) {
    sampleDir_ = dir;
}

void SampleBank::setSampleRate(unsigned int rate) {
    sampleRate_ = rate;
}

void SampleBank::setChannels(unsigned int channels) {
    channels_ = channels;
}

void SampleBank::setSampleSize(unsigned int bytes) {
    switch (bytes) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 8:
            sampleSize_ = bytes;
            break;
        default:
            throw std::runtime_error("Invalid sample size");
    }
}

const std::vector<Word> & SampleBank::getWords() const {
    return words_;
}

unsigned int SampleBank::getSampleSize() const {
    return sampleSize_;
}

unsigned int SampleBank::getSampleRate() const {
    return sampleRate_;
}

unsigned int SampleBank::getChannels() const {
    return channels_;
}

void SampleBank::addSample(const std::string & filename) {
    Sample s(filename);
    samples_.push_back(s);
    words_.insert(words_.end(), s.getWords().begin(), s.getWords().end());
}

ScoreFinder::ScoreFinder() : score_(random()%10000/10000.0) {}

ScoreFinder::ScoreFinder(float score) : score_(score) {}

bool ScoreFinder::operator()(Word & check) const {
    return (abs(check.getScore() - score_) <= 0.00001);
}
        
Word SampleBank::randomWord() {
    if (needsResort_) {
        sort(words_.begin(), words_.end());
        float oldestAge = words_.front().getAge();
        float newestAge = words_.back().getAge();
        for (std::vector<Word>::iterator it = words_.begin(); it != words_.end(); ++it) {
            it->setScore((it->getAge() - oldestAge) / (newestAge - oldestAge));
        }
        needsResort_ = false;
    }
    
    std::vector<Word>::iterator it = find_if(words_.begin(), words_.end(), ScoreFinder());
    float firstScore = words_.front().getScore();
    std::vector<Word> choices;
    while (abs(it->getScore() - firstScore)) {
        choices.insert(choices.end(), *it);
    }
    
    size_t choice = random() % (choices.size() - 1);
    return choices[choice];
}

void SampleBank::initComposition(Composition & comp) {
    comp.words_.clear();
    // pick number between 1 and 17
    // pick that many random words!
    for (int wordcount = 1 + random() % 16; wordcount > 0; --wordcount) {
        comp.words_.push_back(randomWord());
    }
}

