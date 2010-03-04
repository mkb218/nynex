/*
 *  evolvertest.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 2/2/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include <iostream>
#include <cstdlib>
#include "evolvertest.h"
#include "evolver.h"
#include "composition.h"
#include "ratings.h"
#include "ga/ga.h"

using namespace nynex;

int main(int argc, char **argv) {
    SampleBank & bank = SampleBank::getInstance();
    bank.setChannels(2);
    bank.setSampleRate(44100);
    bank.setSampleSize(2);
    bank.setTmpDir("/tmp/nynex");
    bank.setSampleDir("/Users/makane/code/nynex/samples");
    Composition c;
    bank.initComposition(c);
    c.bounceToFile("/Users/makane/code/nynex/output/whatwhat.mp3");
    
    for (int i = 0; i <= 100; ++i) {
        Ratings::getInstance().addRating(c.getObjectId(), random() % 5);
    }
    std::cout << Ratings::getInstance().avgRatingForId(c.getObjectId()) << std::endl;
    
    std::cout << "setting up galib" << std::endl;
    
    Evolver e;
    Ratings & r = Ratings::getInstance();
    e.initGA(0.1, 10, gaFalse);
    while (e.getGA().generation() < 10) {
        const GAPopulation & pop = e.getPop();
        for (size_t i = 0; i < pop.size(); ++i) {
            Composition & c = static_cast<Composition&>(pop.individual(i));
            std::string file("/Users/makane/code/nynex/output/gen");
            file.append(stringFromInt(e.getGA().generation()) + "i" + stringFromInt(i) + ".mp3");
            c.bounceToFile(file);
//            system((std::string("open -a quicktime\\ player ") + file).c_str());
            std::cout << "Enter your numerical opinion: " << std::endl;
            double rating = random() % 5;
            r.addRating(c.getObjectId(), rating);
            std::cout << "Enter someone else's numerical opinion: " << std::endl;
            rating = random() % 5;
            r.addRating(c.getObjectId(), rating);
        }
        e.stepGA();
    }
    
    return 0;
}