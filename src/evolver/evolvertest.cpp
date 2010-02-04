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
    Word word(SampleBank::getInstance().randomWord());
    Sample s("gobbledygook");
    s.getWords();
    Composition c;
    SampleBank::getInstance().initComposition(c);
    return 0;
}