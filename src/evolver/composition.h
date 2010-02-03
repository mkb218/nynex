/*
 *  composition.h
 *  nynex
 *  $Id$
 *
 *  Created by Matthew J Kane on 1/26/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#ifndef NYNEX_COMPOSITION
#define NYNEX_COMPOSITION

#include <vector>
#include <list>
#include <string>

#include "ga/ga.h"

namespace nynex {
    class Word {
    public:
        Word(const std::string & filename, int age);
        Word(const Word &);
        bool operator<(const Word &) const;
        const std::string & getFilename() const;
        int getAge() const;
        float getScore() const;
        unsigned int getDuration() const;
        void setScore(float);
    private:
        void calcDuration();
        std::string filename_;
        int age_;
        unsigned int duration_; // samples
        float score_;
    };
    
    class Sample {
    public:
        Sample(const std::string &);
        Sample(const Sample &);
        Sample & operator=(const Sample & other);
        std::list<Word> getWords() const;
    private:
        void makeWords() const;
        std::list<Word> words;
        std::string filename_;
        int age_;
    };
    
    class Composition;
        
    class SampleBank {
    public:
        static SampleBank & getInstance();
        void setSampleDir(const std::string & dir);
        void setSampleRate(unsigned int rate);
        void setChannels(unsigned int channels);
        void setSampleSize(unsigned int bytes);
        const std::vector<Word> & getWords() const;
        unsigned int getSampleSize() const;
        unsigned int getSampleRate() const;
        unsigned int getChannels() const;
        void addSample(const std::string & filename);
        void setTiebreaker(bool);
        bool getTiebreaker();
        Word randomWord();
        void initComposition(Composition & comp);
    private:
        SampleBank();
        static SampleBank * instance_;
        unsigned int sampleRate_;
        unsigned int channels_;
        unsigned int sampleSize_;
        std::string sampleDir_;
        std::vector<Sample> samples_;
        std::vector<Word> words_;
        bool tiebreaker_;
        mutable bool needsResort_;
    };

    class Composition : public GAGenome {
    public:
        friend void SampleBank::initComposition(Composition &);
        GADefineIdentity("NynexComposition", 219);
        Composition();
        Composition(const std::list<Word> &);
        Composition(const Composition &);
        Composition & operator=(const GAGenome &);
        GAGenome* clone() const;
        void copy(const GAGenome &);
        void bounceToFile(const std::string & filename) const;
        static void init(GAGenome&);
        static int mutate(GAGenome &, float);
        static float compare(const GAGenome &, const GAGenome &);
        static float evaluate(GAGenome &);
        static int crossover(const GAGenome &, const GAGenome &, GAGenome*, GAGenome*);
        //        static void setMidiDevice(MidiDevice&);
        //        static void AudioDevice(AudioDevice&);
    private:
        static unsigned int nextObjectId_;
        const unsigned int objectId_;
        std::list<Word> words_;
        //        static MidiDevice midiout_;
        //        static AudioDevice audioin_;
        //        Fs1rModel fs1rmodel;
        //        std::map<char> notes_;
    };
    
}

#endif