/*
 *  evolvertest.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 2/2/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include "evolvertest.h"
#include "composition.h"
#include "ga/ga.h"

using namespace nynex;

int main(int argc, char **argv) {
    SampleBank & bank = SampleBank::getInstance();
    bank.setChannels(2);
    bank.setSampleRate(48000.0);
    bank.setSampleSize(2);
    bank.setSampleDir("/Users/makane/code/nynex/samplesubset");
    Word word(bank.randomWord());
    Sample s("gobbledygook");
    s.getWords();
    Composition c;
    bank.initComposition(c);
    c.bounceToFile("/Users/makane/code/nynex/output/whatwhat.wav");
    return 0;
}