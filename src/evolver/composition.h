/*
 *  filemgr.h
 *  nynex
 *
 *  Created by Matthew J Kane on 1/26/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#ifndef NYNEX_COMPOSITION
#define NYNEX_COMPOSITION

#include <vector>
#include <string>

#include "ga/ga.h"

namespace nynex {
    class Composition : public GAGenome {
    public:
        GADefineIdentity("NynexComposition", 219);
        Composition();
        Composition(std::vector<Word> &);
        Composition(const Composition &);
        ~Composition();
        Composition & operator=(const GAGenome &);
        GAGenome* clone() const;
        void copy(const GAGenome &);
        std::string bounceToTmpFile(const std::string & tmpDir) const;
        void bounceToFile(const std::string & filename) const;
        static void init(GAGenome&);
        static int mutate(GAGenome &, float);
        static float compare(const GAGenome &, const GAGenome &);
        static float evaluate(GAGenome &);
        static int crossover(const GAGenome &, const GAGenome &, GAGenome*, GAGenome*);
    private:
        static MidiDevice midiout_;
        static AudioDevice audioin_;
        static Ratings *ratings_;
//        Fs1rModel fs1rmodel;
    };
    
    class Word {
    public:
        Word(const std::string & filename, int age);
        std::string & getFilename() const;
        int getAge() const;
    private:
        std::string filename_;
        int age_;
    };
    
    class Sample {
    public:
        Sample(const std::string & filename);
        std::vector<Word> getWords() const;
    private:
        void makeWords() const;
        std::string filename_;
        int age_;
    };
        
    class SampleBank {
    public:
        static getInstance();
        static setSampleDir(const std::string & dir);
        Composition makeComposition() const;
    private:
        SampleBank();
        SampleBank * instance_;
        std::string sampleDir_;
        std::vector<Sample> samples_;
    };
}

#endif