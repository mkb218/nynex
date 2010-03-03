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

extern "C" {
#include "sox.h"
}
#include "ga/ga.h"

#define BUFSIZE 262144 * 16
#define MAXBUFS 64
#define MAXFILEINMEM BUFSIZE * MAXBUFS

namespace nynex {
    class Sample;
    
    class Word {
    public:
        Word(const Sample * parent, size_t index, int age);
        Word(const Word &);
        bool operator<(const Word &) const;
        std::string getFilename() const;
        int getAge() const;
        double getScore() const;
        unsigned int getDuration() const;
        void setScore(double);
    private:
        void calcDuration();
        size_t index_;
        const Sample * parent_;
        int age_;
        unsigned int duration_; // samples
        double score_;
    };

    class WordSorter {
    public:
        bool operator()(Word* left, Word* right) { return (*left) < (*right); }
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
            if (state_ != INMEM)
                buf_ = ((samplebuf_t)malloc(size_*sizeof(sox_sample_t))); 
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
        Sample & operator=(const Sample & other);
        const std::list<Word*> & getWords();
        const std::string & getFilename() const { return filename_; }
    private:
        void makeWords();
        void splitFile();
        std::list<Word*> words_;
        std::string filename_; // relative to sampledir
        int age_;
        bool wordsReady_;
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
    private:
        SampleBank();
        void addSample(const std::string & path);
        static SampleBank * instance_;
        double sampleRate_;
        unsigned int channels_;
        unsigned int sampleSize_;
        std::string sampleDir_;
        std::string tmpDir_;
        std::vector<Sample*> samples_;
        std::vector<Word*> words_;
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
        std::list<Word*> words_;
        //        static MidiDevice midiout_;
        //        static AudioDevice audioin_;
        //        Fs1rModel fs1rmodel;
        //        std::map<char> notes_;
    };
    
}

#endif
