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
#include "ratings.h"
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
#include <dirent.h>
#include <unistd.h>
#include <sys/param.h>
#include <boost/foreach.hpp>
#include <boost/algorithm/string.hpp>

#include <errno.h>

#define MIN_SAMPLESIZE 0.1
#define MAX_SAMPLESIZE 2

using namespace nynex;
using std::sort;

void nynex::mkdir_or_throw(const char * dir) throw(std::runtime_error) {
    struct stat dirinfo;
    if (0 == stat(dir, &dirinfo)) {
        if (!(dirinfo.st_mode & S_IFDIR)) {
            throw std::runtime_error(std::string(dir) + "file exists and is not dir");
        }
    } else {
        if (0 != mkdir(dir, 0700)) {
            throw std::runtime_error("mkdir failed: " + stringFrom(errno));
        }
    }
}

unsigned int Composition::nextObjectId_ = 1;

Composition::Composition() : GAGenome(init, mutate, compare), objectId_(nextObjectId_++) {
    evaluator(evaluate);
    GAGenome::crossover(crossover);
}

Composition::Composition(const std::list<Word*> & words) : GAGenome(init, mutate, compare), objectId_(nextObjectId_++),words_(words.begin(), words.end()) {
    evaluator(evaluate);
    GAGenome::crossover(crossover);
}

Composition::Composition(const std::list<Word*> & words, unsigned int objectId) : GAGenome(init, mutate, compare), objectId_(objectId),words_(words.begin(), words.end()) {
    evaluator(evaluate);
    GAGenome::crossover(crossover);
    if (objectId_ >= nextObjectId_) {
        nextObjectId_ = objectId_+1;
    }
}

Composition & Composition::operator=(const GAGenome & other) {
    if (&other != this) copy(other);
    return *this;
}

void Composition::copy(const GAGenome & other) {
    GAGenome::copy(other);
    const Composition & othercast = dynamic_cast<const Composition &>(other);
    words_.assign(othercast.words_.begin(), othercast.words_.end());
}

GAGenome * Composition::clone(CloneMethod) const {
    // clone is meant to be a distinct entity with different objectid
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
    for (std::list<Word*>::iterator it = trgcast.words_.begin(); it != trgcast.words_.end(); ++it) {
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
    std::list<Word*>::const_iterator leftit = leftcast.words_.begin();
    std::list<Word*>::const_iterator rightit = rightcast.words_.begin();
    while (leftit != leftcast.words_.end() && rightit != rightcast.words_.end()) {
        if ((*leftit)->getFilename() != (*rightit)->getFilename()) {
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

float Composition::evaluate(GAGenome & g) {
    Composition & gcast = dynamic_cast<Composition &>(g);
    return Ratings::getInstance().avgRatingForId(gcast.getObjectId());
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
    std::list<Word*>::const_iterator momit = momcast.words_.begin();
    std::list<Word*>::const_iterator dadit = dadcast.words_.begin();

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
    const sox_signalinfo_t & signal = bank.getSignalInfo();
    sox_format_t *out = sox_open_write(filename.c_str(), &signal, NULL, NULL, NULL, NULL);
    size_t framecount;
    size_t bufsize = bank.getChannels()*BUFSIZE;
    sox_sample_t *buf = (sox_sample_t*)malloc(bufsize*sizeof(sox_sample_t));
    for (std::list<Word*>::const_iterator it = words_.begin(); it != words_.end(); ++it) {
        sox_format_t *in;
        in = sox_open_read((*it)->getFilename().c_str(), &signal, &(bank.getEncodingInfo()), "raw");
        if (!in) { throw std::runtime_error("sox_open failed"); }
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

std::string Composition::serialize() const {
    std::string out(stringFrom(objectId_));
    BOOST_FOREACH(Word *w, words_) {
        out += DELIMITER;
        out.append(w->getParent()->getFilename());
        out += DELIMITER;
        out.append(stringFrom(w->getIndex()));
    }
    return out;
}

class AddWords {
public:
    std::string operator()(const std::string & in) {
        return std::string("words/") + in;
    }
};


Composition * Composition::unserialize(const std::string & src) {
    unsigned int objectId;
    std::list<Word*> words;
    SampleBank & bank = SampleBank::getInstance();
    std::list<std::string> strs;
    boost::split(strs, src, boost::is_from_range(DELIMITER, DELIMITER));
    objectId = fromString<unsigned int>(strs.front());
    strs.pop_front();
    bool even = true;
    Sample * parent;
    BOOST_FOREACH(std::string & s, strs) {
        if (even) {
            parent = bank.getSampleMap().find(s)->second;
        } else {
            size_t index = fromString<size_t>(s);
            std::list<Word*>::const_iterator it = parent->getWords().begin();
            for (size_t i = 0; i < index; ++i) ++it;
            words.push_back(*it);
        }
        even = !even;
    }
    return new Composition(words, objectId);
}

Word::Word(const Sample * parent, size_t index) : index_(index), parent_(parent) {
    calcDuration();
//    std::cout << "Word constructor " << stringFrom((unsigned int)this) << " " << parent_->getFilename() <<std:: endl;
}

Word::Word(const Word & other) : parent_(other.parent_), index_(other.index_), duration_(other.duration_)  {
//    std::cout << "Word copy constructor " << stringFrom((unsigned int)this) << " " << parent_->getFilename() << std::endl;
}

std::string Word::getFilename() const {
    return std::string("words/") + parent_->getFilename() + "/" + stringFrom(index_);
}

bool Sample::operator<(const Sample & other) const {
    return ( age_ < other.age_ );
}

int Word::getAge() const {
    return parent_->getAge();
}

double Word::getScore() const {
    return parent_->getScore();
}

void Sample::setScore(double score) {
    score_ = score;
}

double Sample::getScore() const {
    return score_;
}

double Word::getDuration() const {
    return duration_;
}

void Word::calcDuration() {
    // stat file
    struct stat stats;
    if (0 != stat(getFilename().c_str(), &stats)) {
        throw new std::runtime_error(getFilename()+": stat filed while getting duration");
    }
    SampleBank & bank = SampleBank::getInstance();
    duration_ = ((double)stats.st_size); // bytes / file
    duration_ /= bank.getSampleSize(); // frames / file
    duration_ /= bank.getChannels(); // samples / file
    duration_ /= bank.getSampleRate(); // seconds / file
}

Sample::Sample(const std::string & filename) : wordsReady_(false),filename_(filename), score_(0.) {
    chdir(SampleBank::getInstance().getSampleDir().c_str());
    struct stat age_s;
    if (0 == stat(filename_.c_str(),&age_s)) {
        age_ = age_s.st_mtime;
    } else {
        throw std::runtime_error("couldn't stat file " + filename + " given as sample file");
    }
}

Sample::Sample(const Sample & other) : filename_(other.filename_),age_(other.age_),wordsReady_(other.wordsReady_),words_(other.words_), score_(0.) {
}

Sample::~Sample() {
    while (!words_.empty()) {
        delete words_.front();
        words_.pop_front();
    }
}

Sample & Sample::operator=(const Sample & other) {
    filename_ = other.filename_;
    age_ = other.age_;
    wordsReady_ = false;
}

const std::list<Word*> & Sample::getWords() {
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
            words_.push_front(new Word(this, files));
        }
    } else {
        splitFile();
    }
    wordsReady_ = true;
}

static inline int32_t to8bit(sox_sample_t s) {
    SOX_SAMPLE_LOCALS;
    int clips = 0;
    return SOX_SAMPLE_TO_UNSIGNED_8BIT(s, clips);
}

static inline int32_t to16bit(sox_sample_t s) {
    SOX_SAMPLE_LOCALS;
    int clips = 0;
    return SOX_SAMPLE_TO_SIGNED_16BIT(s, clips);
}

static inline int32_t to24bit(sox_sample_t s) {
    SOX_SAMPLE_LOCALS;
    int clips = 0;
    return SOX_SAMPLE_TO_SIGNED_24BIT(s, clips);
}

static inline int32_t to32bit(sox_sample_t s) {
    SOX_SAMPLE_LOCALS;
    int clips = 0;
    return SOX_SAMPLE_TO_SIGNED_24BIT(s, clips);
}

void Sample::splitFile() {
    std::cout << "splitting sample " << filename_ << std::endl;
    static int32_t(*functable[5])(sox_sample_t) = {NULL, to8bit, to16bit, to24bit, to32bit};
    SampleBank & bank = SampleBank::getInstance();
    sox_format_t *in;
    std::list<SplitBuf* > buf;
    chdir(bank.getSampleDir().c_str());

    size_t read;
    size_t bufsize = BUFSIZE;
    size_t floor, exit;
    {
        struct stat junk;
        int stat_status = stat(filename_.c_str(), &junk);
        if (stat_status != 0) {
            throw std::runtime_error(std::string("stat failed ") + stringFrom(stat_status));
        }
        bool unloadBufs = (junk.st_size > MAXFILEINMEM);
        in = sox_open_read(filename_.c_str(), &(bank.getSignalInfo()), NULL, NULL);

        if (bufsize % bank.getChannels()) {
            bufsize *= bank.getChannels();// needs to be a multiple of number of samples in a frame
        }
        
        read = 0; // need this later
        do {
            SplitBuf *newbuf = new SplitBuf(bufsize*sizeof(sox_sample_t));
            buf.push_back(newbuf);
            read = sox_read(in, *newbuf, bufsize);
            if (unloadBufs) {
                buf.back()->unload();
            }
        } while (bufsize == read);
        sox_close(in);
    
        // find mean of abs values
        double sum = 0.;
        size_t count = 0;
        size_t list_ix = 0;
        size_t limit = bufsize;
        for (std::list<SplitBuf*>::iterator it = buf.begin(); it != buf.end(); ++it) {
            if (list_ix + 1 == buf.size()) {
                limit = read;
            }
        
            for (size_t ix = 0; ix < bufsize; ++ix) {
                ++count;
                sum += abs(functable[bank.getSampleSize()]((**it)[ix]));
            }
            if (unloadBufs) {
                (*it)->unload();
            }
        }
    
        double mean = sum/count;
        floor = 0.15 * mean;
        exit = floor * 0.2;
    }
    
    const size_t lastbufsize = read;
    
    size_t minSampleSize = MIN_SAMPLESIZE * bank.getSampleRate() * bank.getChannels();
    size_t maxSampleSize = MAX_SAMPLESIZE * bank.getSampleRate() * bank.getChannels();
    size_t word_ix = 0;
    std::string filebase(filename_);
    std::string filename(filebase+"/"+stringFrom(word_ix));

    chdir(bank.getSampleDir().c_str());
    mkdir_or_throw("words");
    mkdir_or_throw((std::string("words/")+filebase).c_str());
    sox_format_t * out = sox_open_write(("words/"+filename).c_str(), &(bank.getSignalInfo()), &(bank.getEncodingInfo()), "raw", NULL, NULL);
    size_t list_ix = 0;
    size_t limit = bufsize;
    bool inGap = true;
    bool outputReady = false;
    
    while ( !buf.empty() ) {
        size_t lastWriteEnd = 0; // also the beginning of the current word
      
        if (1 == buf.size()) {
            limit = lastbufsize;
        }

        for (size_t frameix = 0; frameix < limit; frameix += bank.getChannels()) {
            double s = 0;
            for (size_t sampleix = 0; sampleix < bank.getChannels(); ++sampleix) {
                double sampleval = functable[bank.getSampleSize()]((*buf.front())[frameix+sampleix]);
                s += (fabs(sampleval) / bank.getChannels());
            }
            
            if (!inGap && (frameix - lastWriteEnd > minSampleSize) && ((s < exit) || (frameix - lastWriteEnd) > maxSampleSize)) {
                // write sample
                if (!outputReady) {
                    out = sox_open_write(("words/"+filename).c_str(), &(bank.getSignalInfo()), &(bank.getEncodingInfo()), "raw", NULL, NULL);
                    outputReady = true;
                }
                sox_write(out, *(buf.front()) + lastWriteEnd, frameix - lastWriteEnd);
                if (frameix - lastWriteEnd == 0) {
                    std::cout << "what1";
                }
                words_.push_back(new Word(this, word_ix));
                ++word_ix;
                sox_close(out);
                outputReady = false;
                filename = filebase+"/"+stringFrom(word_ix);
                lastWriteEnd = frameix;
                inGap = (s < exit);
            } else if (inGap && s > floor) {
                lastWriteEnd = frameix;
                inGap = false;
            }
            
        }
        
        if (!inGap && limit - lastWriteEnd > 0) {
            if (!outputReady) {
                out = sox_open_write(("words/"+filename).c_str(), &(bank.getSignalInfo()), &(bank.getEncodingInfo()), "raw", NULL, NULL);
                outputReady = true;
            }
            if (limit - lastWriteEnd == 0) {
                std::cout << "what2";
            }
            sox_write(out, *(buf.front())+lastWriteEnd, limit - lastWriteEnd);
        }
        delete buf.front();
        buf.pop_front();
    }
    
    if (outputReady) {
        sox_close(out);
        words_.push_back(new Word(this, word_ix));
    } else {
        --word_ix;
    }
    
    mkdir_or_throw("indexes");
    std::ofstream stream(("indexes/"+filebase+".index").c_str());
    stream << (word_ix+1) << std::endl;
    stream.close();    
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

const sox_signalinfo_t & SampleBank::getSignalInfo() const {
    if (!signalready_) {
        soxsignal_.rate = getSampleRate();
        soxsignal_.channels = getChannels();
        soxsignal_.precision = getSampleSize() * 8;
        soxsignal_.length = SOX_IGNORE_LENGTH;
        soxsignal_.mult = NULL;
        signalready_ = true;
    }
    return soxsignal_;
}

SampleBank::SampleBank() {
    srandomdev();
    sox_format_init();
    encodingready_ = false;
    needsResort_ = true;
}
                       
SampleBank::~SampleBank() {
    while (!samples_.empty()) {
        delete samples_.back();
        samples_.pop_back();
    }
    sox_format_quit();
}

void SampleBank::setTmpDir(const std::string & dir) {
    tmpDir_ = dir;
}

void SampleBank::setSampleDir(const std::string & dir) {
    sampleDir_ = dir;
    encodingready_ = false;
    signalready_ = false;
    chdir(dir.c_str());
    DIR *d = opendir(dir.c_str());
    dirent *e = NULL;
    while ((e = readdir(d)) != NULL) {
        // stat and mode & 0xfff0000 should be 0
        struct stat dirinfo;
        if (0 != stat(e->d_name,&dirinfo)) {
            throw std::runtime_error(std::string("couldn't stat file we know exists: ") + e->d_name + " errno " + stringFrom(errno));
        }
        if (dirinfo.st_mode & S_IFDIR) {
            continue;
        }
        
        // TODO one thread per CPU
        addSample(e->d_name);
    }
    std::cout << "done adding sample" << std::endl;
    closedir(d);
}

void SampleBank::setSampleRate(double rate) {
    sampleRate_ = rate;
    encodingready_ = false;
    signalready_ = false;
}

void SampleBank::setChannels(unsigned int channels) {
    channels_ = channels;
    encodingready_ = false;
    signalready_ = false;
}

void SampleBank::setSampleSize(unsigned int bytes) {
    switch (bytes) {
        case 1:
        case 2:
        case 3:
        case 4:
        case 8:
            sampleSize_ = bytes;
            signalready_ = false;
            encodingready_ = false;
            break;
        default:
            throw std::runtime_error("Invalid sample size");
    }
}

const std::vector<Word*> & SampleBank::getWords() const {
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
    std::cout << "adding sample " << filepath << std::endl;
    chdir(getSampleDir().c_str());
    sox_format_t *in;
    in = sox_open_read(filepath.c_str(), &soxsignal_, NULL, NULL);
    if (in != NULL) {
        sox_close(in);
        Sample *s;
        try {
#ifdef __DARWIN_UNIX03
            char * tmp = new char[filepath.size()+1];
            strncpy(tmp, filepath.c_str(), filepath.size()+1);
            s = new Sample(basename(tmp));
            delete [] tmp;
#else
            s = new Sample(basename(filepath.c_str()));
#endif
        } catch (std::bad_alloc& e) {
            throw std::runtime_error("creating new sample threw bad_alloc. dying.");
        }
        samples_.push_back(s);
        words_.reserve(s->getWords().size());
        sampleMap_.insert(make_pair(s->getFilename(),s));
        BOOST_FOREACH(Word *w, s->getWords()) {
            words_.push_back(w);
        }
        needsResort_ = true;
    }
}

//ScoreFinder::ScoreFinder() : score_(random()%10000/10000.0) {}
//
//ScoreFinder::ScoreFinder(double score) : score_(score) {}
//
//bool ScoreFinder::operator()(Word & check) const {
//    return (score_ - check.getScore() <= 0.00001);
//}
//        
Word* SampleBank::randomWord() {
    if (needsResort_) {
        sort(samples_.begin(), samples_.end(), SampleSorter());
        double oldestAge = samples_.front()->getAge();
        double interval = oldestAge * INTERVAL;
        oldestAge -= interval;
        double newestAge = samples_.back()->getAge() + interval;
        double slope = 1 / (newestAge - oldestAge);
        double yInt = - oldestAge * slope;
        for (std::vector<Sample*>::iterator it = samples_.begin(); it != samples_.end(); ++it) {
            if (oldestAge == newestAge) {
                (*it)->setScore(0.);
            } else {
                double age = (*it)->getAge();
                double score = slope * age + yInt;
                if (score != score) { // NaN
                    score = 0;
                }
                
//                std::cout << score << std::endl;
                (*it)->setScore(score);
            }
        }
        needsResort_ = false;
    }
    
    // this ain't working, but need word index file first to test more easily.
    size_t ix;
    do {
        ix = random() % words_.size();
    } while ((words_[ix])->getScore() <= random() % 10000 / 10000); 

//    std::cout << (words_[ix])->getFilename() << " score " << (words_[ix])->getScore() << std::endl;
    return (words_[ix]);
}

void SampleBank::initComposition(Composition & comp) {
    comp.words_.clear();
    // we want at least MINDURATION seconds
    double duration = 0.;
    while (duration <= MINDURATION) {
        Word * w = randomWord();
        duration += w->getDuration();
        comp.words_.push_back(w);
    }
}

void SplitBuf::load() const {
    if (state_ != ONDISK) throw std::logic_error("load called with no tmp file"); 
    alloc();
    sox_format_t *in = sox_open_read(filename().c_str(), &(SampleBank::getInstance().getSignalInfo()), &(SampleBank::getInstance().getEncodingInfo()), "raw");
    if (in == NULL) {
        throw std::runtime_error("couldn't open tmp file");
    }
    
    if (size_ != sox_read(in, buf_, size_)) {
        throw std::runtime_error("couldn't read whole file");
    }
    sox_close(in);
    rmtmp();
    state_ = INMEM;
}

void SplitBuf::unload() const {
    if (state_ != INMEM) throw std::logic_error("unload called with no buffer");
    mkdir_or_throw(SampleBank::getInstance().getTmpDir().c_str());
    sox_format_t *out = sox_open_write(filename().c_str(), &(SampleBank::getInstance().getSignalInfo()), &(SampleBank::getInstance().getEncodingInfo()), "raw", NULL, NULL);
    if (size_ != sox_write(out, buf_, size_)) {
        throw std::runtime_error("couldn't write whole file");
    }
    sox_close(out);
    dealloc();
    state_ = ONDISK;
}

std::string SplitBuf::filename() const {
    return SampleBank::getInstance().getTmpDir() + "/" + stringFrom((size_t)this);
}

void SplitBuf::rmtmp() const {
    struct stat junk;
    std::string file(filename());
    int stat_status = stat(file.c_str(), &junk);
    if (stat_status == ENOENT || (stat_status == 0 && unlink(file.c_str()) != 0)) {
        throw std::runtime_error(std::string("couldn't unlink tmp file ") + file);
    }
}

