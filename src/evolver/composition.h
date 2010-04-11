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
#include <memory>
#include <string>
#include <sstream>
#include <map>
#include <boost/ref.hpp>
#include <boost/unordered_map.hpp>

extern "C" {
#include "sox.h"
}
#include "ga/ga.h"

#define BUFSIZE (1 << 24)
#define MAXBUFS 32
#define MAXFILEINMEM BUFSIZE * MAXBUFS
#define INTERVAL 0.1
#define MINDURATION 15.0
#define DELIMITER ':'

template <class Type>
std::string stringFrom(Type i) {
    std::ostringstream os;
    os << i;
    return os.str();
}

template <class Type>
Type fromString(const std::string & i) {
    std::istringstream is;
    Type num;
    is.str(i);
    is >> num;
    return num;
}


namespace nynex {
    class Sample;
    
    class Word {
    public:
        Word(const Sample * parent, size_t index);
        Word(const Word &);
        std::string getFilename() const;
        int getAge() const;
        double getScore() const;
        double getDuration() const;
        const Sample * getParent() { return parent_; }
        size_t getIndex() { return index_; }
    private:
        void calcDuration();
        size_t index_;
        const Sample * parent_;
        double duration_; // samples
    };

    typedef sox_sample_t* samplebuf_t;
    class SplitBuf {
    public:
        explicit SplitBuf(size_t size) : size_(size), state_(DEAD) { alloc(); state_ = INMEM; }
        ~SplitBuf() { 
            try { 
                rmtmp(); 
                dealloc(); 
            } catch (...) {} 
        }
        SplitBuf & operator=(SplitBuf & other) { 
            other.load();
            size_ = other.size_;
            buf_ = other.buf_; 
            state_ = other.state_;
            other.rmtmp(); 
            other.state_ = DEAD; 
        }
        operator samplebuf_t() {
            if (state_ == ONDISK) load(); 
            return buf_; 
        }
        void load() const; // alloc(), read in from file, rmtmp()
        void unload() const; // save to file and dealloc();
    private:
        enum State {
            ONDISK,
            INMEM,
            DEAD
        } ;
        mutable State state_;
        void alloc() const {
            if (state_ != INMEM) {
                buf_ = ((samplebuf_t)malloc(size_*sizeof(sox_sample_t))); 
                if (buf_ == NULL) {
                    throw std::runtime_error("SplitBuf alloc failed!");
                }
            }
        }
        void dealloc() const {
            if (state_ == INMEM)
                free(buf_); 
        }
        void rmtmp() const; 
        std::string filename() const;
        static unsigned int bufs;
        mutable samplebuf_t buf_;
        size_t size_;
    };
    
    class Sample {
    public:
        Sample(const std::string &);
        Sample(const Sample &);
        ~Sample();
        bool operator<(const Sample &) const;
        Sample & operator=(const Sample & other);
        int getAge() const { return age_; }
        void setScore(double);
        double getScore() const;
        const std::list<Word*> & getWords();
        const std::string & getFilename() const { return filename_; }
    private:
        void makeWords();
        void splitFile();
        std::list<Word*> words_;
        std::string filename_; // relative to sampledir
        int age_;
        double score_;
        bool wordsReady_;
    };

    class SampleSorter {
    public:
        bool operator()(Sample* left, Sample* right) { return (*left) < (*right); }
    };
    
    class Composition;
        
    class SampleBank {
    public:
        static SampleBank & getInstance();
        ~SampleBank();
        void setSampleDir(const std::string & dir);
        void setSampleRate(double rate);
        void setChannels(unsigned int channels);
        void setSampleSize(unsigned int bytes);
        void setTmpDir(const std::string & tmpdir);
        const std::string & getSampleDir() const { return sampleDir_; }
        std::string getWordDir() const { return sampleDir_+"/words"; }
        const std::vector<Word*> & getWords() const;
        unsigned int getSampleSize() const;
        double getSampleRate() const;
        unsigned int getChannels() const;
        const std::string & getTmpDir() const { return tmpDir_; }
        Word *randomWord();
        void initComposition(Composition & comp);
        const sox_encodinginfo_t & getEncodingInfo() const;
        const sox_signalinfo_t & getSignalInfo() const;
        const std::map<std::string, Sample*> & getSampleMap() { return sampleMap_; }
    private:
        SampleBank();
        SampleBank(SampleBank &) {} // never called
        SampleBank & operator=(SampleBank &) {return *this;} // never called
        void addSample(const std::string & path);
        static SampleBank * instance_;
        double sampleRate_;
        unsigned int channels_;
        unsigned int sampleSize_;
        std::string sampleDir_;
        std::string tmpDir_;
        std::vector<Sample*> samples_;
        std::vector<Word*> words_;
        std::map<std::string, Sample*> sampleMap_;
        mutable sox_encodinginfo_t soxencoding_;
        mutable bool encodingready_;
        mutable sox_signalinfo_t soxsignal_;
        mutable bool signalready_;
        mutable bool needsResort_;
    };

    class Composition : public GAGenome {
    public:
        friend void SampleBank::initComposition(Composition &);
        GADefineIdentity("NynexComposition", 219);
        Composition();
        Composition(const std::list<Word*> &);
        Composition & operator=(const GAGenome &);
        GAGenome* clone(CloneMethod flag = CONTENTS) const;
        void copy(const GAGenome &);
        std::string serialize() const;
        void bounceToFile(const std::string & filename) const;
        unsigned int getObjectId() const { return objectId_; }
        static Composition* unserialize(const std::string &);
        static void init(GAGenome&);
        static int mutate(GAGenome &, float);
        static float compare(const GAGenome &, const GAGenome &);
        static float evaluate(GAGenome &);
        static int crossover(const GAGenome &, const GAGenome &, GAGenome*, GAGenome*);
        //        static void setMidiDevice(MidiDevice&);
        //        static void AudioDevice(AudioDevice&);
    private:
        Composition(const std::list<Word*> &, unsigned int objectId);
        static unsigned int nextObjectId_;
        const unsigned int objectId_;
        std::list<Word*> words_;
        //        static MidiDevice midiout_;
        //        static AudioDevice audioin_;
        //        Fs1rModel fs1rmodel;
        //        std::map<char> notes_;
    };
    
}

#endif
