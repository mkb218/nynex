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
#include <fstream>
#include <map>
#include <stdexcept>
#include <limits>
#include <algorithm>
#include <cmath>
#include <sys/types.h>
#include <sys/stat.h>
#include <libgen.h>
#include <sstream>
#include <dirent.h>
#include <errno.h>

using namespace nynex;
using std::sort;

template <class IntegerType>
std::string stringFromInt(IntegerType i) {
    std::ostringstream is;
    is << i;
    return is.str();
}

static void mkdir_or_throw(const char * dir) throw(std::runtime_error) {
    struct stat dirinfo;
    if (0 == stat(dir, &dirinfo)) {
        if (!(dirinfo.st_mode & S_IFDIR)) {
            throw std::runtime_error(std::string(dir) + "file exists and is not dir");
        }
    } else {
        if (!mkdir(dir, 0700)) {
            throw std::runtime_error("mkdir failed: " + stringFromInt(errno));
        }
    }
}

unsigned int Composition::nextObjectId_ = 1;
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
        trgcast.words_.pop_front();
    }
    
    // roll dice on whether to remove sample from end
    // random float < p means remove end sample
    if (random() % 10000 / 10000.0 > p) {
        trgcast.words_.pop_back();
    }
    
    // iterate through words, each time rolling dice to pick a new one
    // random float < p means choose a random new sample to replace it
    for (std::list<Word>::iterator it = trgcast.words_.begin(); it != trgcast.words_.end(); ++it) {
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
    SampleBank::getInstance(); // ensure srandomdev has been called
    const Composition & momcast = dynamic_cast<const Composition &>(mom);
    const Composition & dadcast = dynamic_cast<const Composition &>(dad);
    Composition * brocast = NULL; 
    if (bro != NULL) {
        brocast = dynamic_cast<Composition *>(bro);
        brocast->words_.clear();
    }
    
    Composition * siscast = NULL; 
    if (sis != NULL) {
        siscast = dynamic_cast<Composition *>(sis);
        siscast->words_.clear();
    }
    
    size_t crossover = random() % std::min(momcast.words_.size(), dadcast.words_.size());
    std::list<Word>::const_iterator momit = momcast.words_.begin();
    std::list<Word>::const_iterator dadit = dadcast.words_.begin();

    for (size_t i = 0; i < std::max(momcast.words_.size(), dadcast.words_.size()) ; ++i) {
        if (i < crossover) {
            // mom's words to left of crossover point go to beginning of bro
            // dad's words from left of crossover point go to beginning of sis
            if (brocast != NULL && i < momcast.words_.size()) {
                brocast->words_.push_back(*momit);
            }
            if (siscast != NULL && i < dadcast.words_.size()) {
                siscast->words_.push_back(*dadit);
            }
        } else {
            // dad's words from right of crossover point go to end of bro
            // mom's words from right of crossover point go to end of sis
            if (brocast != NULL && i < dadcast.words_.size()) {
                brocast->words_.push_back(*dadit);
            }
            if (siscast != NULL && i < momcast.words_.size()) {
                siscast->words_.push_back(*momit);
            }
        }
        
        if (i < dadcast.words_.size()) {
            ++dadit;
        }

        if (i < momcast.words_.size()) {
            ++momit;
        }
    }
    // crossover fs1rgen
    // choose crossover point <= min(mom.notes_.size(), dad.notes_.size()) and >= 0
    // same pattern as for words
}

void Composition::bounceToFile(const std::string & filename) const {
    // concatenate all words to libsox to filename with format autodetect
    SampleBank & bank = SampleBank::getInstance();
    chdir(bank.getSampleDir().c_str());
    sox_signalinfo_t signal;
    signal.rate = bank.getSampleRate();
    signal.channels = bank.getChannels();
    signal.precision = bank.getSampleSize() * 8;
    signal.length = SOX_IGNORE_LENGTH;
    signal.mult = NULL;
    sox_format_t *out = sox_open_write(filename.c_str(), &signal, NULL, NULL, NULL, NULL);
    size_t framecount;
    size_t bufsize = bank.getChannels()*1024;
    sox_sample_t *buf = (sox_sample_t*)malloc(bufsize*sizeof(sox_sample_t));
    for (std::list<Word>::const_iterator it = words_.begin(); it != words_.end(); ++it) {
        sox_format_t *in;
        in = sox_open_read(it->getFilename().c_str(), &signal, &(bank.getEncodingInfo()), "raw");
        size_t read;
        while (bufsize == (read = sox_read(in,buf,bufsize))) {
            sox_write(out, buf, read);
        }
        sox_write(out,buf,read);
        sox_close(in);
    }
    sox_close(out);
    free(buf);
    // calculate length of sample
    // hold notes for length of sample while recording on audio in
}

Word::Word(const std::string & filename, int age) : filename_(filename), age_(age), score_(0.0) {
    calcDuration();
}

Word::Word(const Word & other) : filename_(other.filename_), age_(other.age_), score_(other.score_), duration_(other.duration_)  {
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

double Word::getScore() const {
    return score_;
}

void Word::setScore(double score) {
    score_ = score;
}

unsigned int Word::getDuration() const {
    return duration_;
}

void Word::calcDuration() {
    // ask libsox for duration of filename
}

Sample::Sample(const std::string & filename) : wordsReady_(false),filename_(filename) {
    chdir(SampleBank::getInstance().getSampleDir().c_str());
    struct stat age_s;
    if (0 == stat(filename_.c_str(),&age_s)) {
        age_ = age_s.st_mtime;
    } else {
        throw std::runtime_error("couldn't stat file " + filename + " given as sample file");
    }
}

Sample::Sample(const Sample & other) : filename_(other.filename_),age_(other.age_),wordsReady_(false) {
}

Sample & Sample::operator=(const Sample & other) {
    filename_ = other.filename_;
    age_ = other.age_;
    wordsReady_ = false;
}

const std::list<Word> & Sample::getWords() {
    if (!wordsReady_) {
        makeWords();
    }
    return words_;
}

void Sample::makeWords() {
    // read in sample data to buffer
    SampleBank & bank = SampleBank::getInstance();
    // TODO IMPORTANT write file tracking words to samples, don't keep re-splitting samples each run
    chdir(bank.getSampleDir().c_str());
    static std::string indexdir = "indexes";
    std::string indexfile = indexdir + "/" + filename_ + ".index";
    struct stat junk;
    
    if (0 == stat(indexfile.c_str(), &junk)) {
        int files;
        std::ifstream stream(indexfile.c_str());
        if (stream.fail()) {
            throw std::runtime_error("index file open failed for " + indexfile);
        }
        stream >> files;
        stream.close();
        while (files > 0) {
            --files;
            words_.push_front(Word("words/"+filename_+"."+stringFromInt(files), age_));
        }
    } else {
        splitFile();
    }
    wordsReady_ = true;
}

void Sample::splitFile() {
    SampleBank & bank = SampleBank::getInstance();
    sox_format_t *in;
    std::list<sox_sample_t *> buf;
    sox_signalinfo_t signal;
    signal.rate = bank.getSampleRate();
    signal.channels = bank.getChannels();
    signal.precision = bank.getSampleSize() * 8;
#if SOX_LIB_VERSION_CODE >= SOX_LIB_VERSION(14,3,0)
    signal.length = SOX_IGNORE_LENGTH;
    signal.mult = NULL;
#else
    signal.length = 0;
#endif
    chdir(bank.getSampleDir().c_str());
    in = sox_open_read(filename_.c_str(), &signal, NULL, NULL);
    
    size_t bufsize = 1024 * bank.getChannels(); // needs to be a multiple of number of samples in a frame
    size_t read = 0; // need this later
    do {
        buf.push_back((sox_sample_t*)malloc(bufsize*sizeof(sox_sample_t)));
        read = sox_read(in, buf.back(), bufsize);
    } while (bufsize == read);
    sox_close(in);
    
    // find mean of abs values
    double sum = 0.;
    size_t count = 0;
    size_t list_ix = 0;
    size_t limit = bufsize;
    for (std::list<sox_sample_t*>::iterator it = buf.begin(); it != buf.end(); ++it) {
        if (list_ix + 1 == buf.size()) {
            limit = read;
        }
        
        for (size_t ix = 0; ix < bufsize; ++ix) {
            ++count;
            sum += abs((*it)[ix]);
        }
    }
    
    double mean = sum/count;
    sox_sample_t floor = 0.01 * mean;
    const size_t lastbufsize = read;
    
    // if more than 0.01 s is below this level eliminate those samples
    size_t gapsize = 0.01 * bank.getSampleRate() * bank.getChannels(); // s * frames/s * samples/frame
    size_t maxSampleSize = gapsize * 50;
    // all samples between gaps are put in own files
    size_t currentGapLength = 0;
    size_t currentSampleLength = 0;
    size_t word_ix = 0;
    std::string filebase(filename_+"."); // TODO kill extension
    std::string filename(filebase+stringFromInt(word_ix));
    chdir(bank.getSampleDir().c_str());
    mkdir_or_throw("words");
    sox_format_t * out = sox_open_write(("words/"+filename).c_str(), &signal, &(bank.getEncodingInfo()), "raw", NULL, NULL);
    list_ix = 0;
    limit = bufsize;
    for (std::list<sox_sample_t*>::iterator it = buf.begin(); it != buf.end(); ++it) {
        size_t offset = 0;
        size_t lastwrite = 0;
        if (list_ix + 1 == buf.size()) {
            limit = lastbufsize;
        }
        
        bool sampleend = false;
        for (size_t sampleix = 0; sampleix < limit; ++sampleix) {
            ++currentSampleLength;
            sox_sample_t s = (*it)[sampleix];
            if (abs(s) < floor) {
                ++currentGapLength;
                ++offset;
            } else {
                currentGapLength = 0;
            }
            
            sampleend = sampleend || (currentGapLength > gapsize || currentSampleLength > maxSampleSize);
            
            // this is outside the innermost loop to make sure 
            // that gaps are aligned on FRAME boundaries
            if (sampleix % bank.getChannels() == bank.getChannels() - 1 && // last sample in frame
                sampleend) {
                sox_write(out, (*it) + offset + lastwrite, sampleix * bank.getChannels() - offset - lastwrite);
                sox_close(out);
                // make a new word with each file
                words_.push_back(Word("words/"+filename, age_));
                ++word_ix;
                filename = filebase+stringFromInt(word_ix);
                out = sox_open_write(("words/"+filename).c_str(), &signal, &(bank.getEncodingInfo()), "raw", NULL, NULL);
                lastwrite = sampleix * bank.getChannels();
                offset = 0;
                
                sampleend = false;
                currentGapLength = 0;
                currentSampleLength = 0;
            }             
        }
        sox_write(out, (*it)+offset, limit - offset);
    }
    sox_close(out);
    words_.push_back(Word("words/"+filename, age_));
    
    mkdir_or_throw("indexes");
    std::ofstream stream(("indexes/"+filebase+"index").c_str());
    stream << (word_ix+1) << std::endl;
    stream.close();    
    
    // TODO make this more elegant
    while (!buf.empty()) {
        delete [] buf.front();
        buf.pop_front();
    }
}    

SampleBank* SampleBank::instance_ = NULL;

SampleBank & SampleBank::getInstance() {
    if (instance_ == NULL) {
        instance_ = new SampleBank();
    }
    return *instance_;
}
    
const sox_encodinginfo_t & SampleBank::getEncodingInfo() const {
    if (!encodingready_) {
        soxencoding_.encoding = SOX_ENCODING_SIGN2;
        soxencoding_.bits_per_sample = sampleSize_ * 8;
        soxencoding_.reverse_bytes = SOX_OPTION_DEFAULT;
        soxencoding_.reverse_nibbles = SOX_OPTION_DEFAULT;
        soxencoding_.reverse_bits = SOX_OPTION_DEFAULT;
        encodingready_ = true;
    }
    return soxencoding_;
}

SampleBank::SampleBank() {
    srandomdev();
    sox_format_init();
    encodingready_ = false;
    needsResort_ = true;
}
                       
SampleBank::~SampleBank() {
    sox_format_quit();
}

void SampleBank::setSampleDir(const std::string & dir) {
    sampleDir_ = dir;
    encodingready_ = false;
    chdir(dir.c_str());
    DIR *d = opendir(dir.c_str());
    dirent *e = NULL;
    while ((e = readdir(d)) != NULL) {
        // stat and mode & 0xfff0000 should be 0
        struct stat dirinfo;
        if (0 != stat(e->d_name,&dirinfo)) {
            throw std::runtime_error(std::string("couldn't stat file we know exists: ") + e->d_name + " errno " + stringFromInt(errno));
        }
        if (dirinfo.st_mode & S_IFDIR) {
            continue;
        }
        
        // TODO one thread per CPU
        addSample(e->d_name);
    }
    closedir(d);
}

void SampleBank::setSampleRate(double rate) {
    sampleRate_ = rate;
    encodingready_ = false;
}

void SampleBank::setChannels(unsigned int channels) {
    channels_ = channels;
    encodingready_ = false;
}

void SampleBank::setSampleSize(unsigned int bytes) {
    switch (bytes) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 8:
            sampleSize_ = bytes;
            encodingready_ = false;
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

double SampleBank::getSampleRate() const {
    return sampleRate_;
}

unsigned int SampleBank::getChannels() const {
    return channels_;
}

void SampleBank::addSample(const std::string & filepath) {
    Sample s(basename(filepath.c_str()));
    samples_.push_back(s);
    words_.insert(words_.end(), s.getWords().begin(), s.getWords().end());
    needsResort_ = true;
}

//ScoreFinder::ScoreFinder() : score_(random()%10000/10000.0) {}
//
//ScoreFinder::ScoreFinder(double score) : score_(score) {}
//
//bool ScoreFinder::operator()(Word & check) const {
//    return (score_ - check.getScore() <= 0.00001);
//}
//        
Word SampleBank::randomWord() {
    if (needsResort_) {
        sort(words_.begin(), words_.end());
        double oldestAge = words_.front().getAge();
        double newestAge = words_.back().getAge();
        for (std::vector<Word>::iterator it = words_.begin(); it != words_.end(); ++it) {
            double score = (it->getAge()/2 - oldestAge) / (newestAge - oldestAge) + 1;
            if (score != score) { // NaN
                score = 0;
            }
            
            it->setScore(score);
        }
        needsResort_ = false;
    }
    
    // this ain't working, but need word index file first to test more easily.
    std::vector<Word>::iterator it = words_.begin();
    size_t ix = 0;
    while (ix + 1 < words_.size()) { 
        double nextWordChance = (words_.size() - (double)ix - 1) / words_.size();
        double dice = (double)(random() % 10000 / 10000.0);
        if (dice > nextWordChance) {
            break;
        }
        ++it;
        ++ix;
    }
    double firstScore = words_.front().getScore();

    return *it;
}

void SampleBank::initComposition(Composition & comp) {
    comp.words_.clear();
    // pick number between 1 and 17
    // pick that many random words!
    for (int wordcount = (random() % 17 + 17); wordcount > 0; --wordcount) {
        comp.words_.push_back(randomWord());
    }
}

