#include "app.h"
#include "ratings.h"

#include <sys/stat.h>

using namespace nynex;

// app has following states
// 0.) init ga, bounce all comps to files and play
// 1.) we play all compositions in sequence, timer starts. once playing is done, commence step 2. display running countdown.
// 2.) display buttons for each composition allowing users to listen. display running countdown
// 3.) if user clicks listen button, display rating buttons plus cancel button. rating times out after 5 minutes
// 4a.) user clicks rating button, rating stored, back to step 2
// 4b.) user clicks cancel button, no rating stored, back to step 2
// 5.) time's up, retrieve web ratings. if there are ratings, step ga and bounce, otherwise just go back to step 1

static std::string fileForGenAndIndividual(int gen, int i) {
    return std::string("gen")+stringFrom(gen)+"i"+stringFrom(i)+".wav";
}

//--------------------------------------------------------------
void nynexApp::setup(){
    // fullscreen
//    ofSetFullscreen(true);
    
    // set background color
    ofBackground(BG_R, BG_G, BG_B);
    
    // set frame rate + vert refresh
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    
    // init config if file exists
    ifstream is(configpath_.c_str());
    if (!is.fail()) {
        config_.setFromStream(is);
    }
    
    // must get this from config!
    SampleBank::getInstance().setSampleRate(fromString<double>(config_.kvp["samplerate"]));
    SampleBank::getInstance().setSampleSize(fromString<int>(config_.kvp["samplesize"]));
    SampleBank::getInstance().setChannels(fromString<int>(config_.kvp["channels"]));
    SampleBank::getInstance().setSampleDir(samplepath_);
    
    // init ga (from file if exists)
    evolver_ = new Evolver();
    struct stat junk;
    if (0 == stat(config_.kvp["gastatefile"].c_str(), &junk)) {
        try {
            evolver_->loadFromFile(config_.kvp["gastatefile"]);
        } catch ( ... ) {
            evolver_->initGA(fromString<float>(config_.kvp["pMutation"]), fromString<int>(config_.kvp["popSize"]), fromString<bool>(config_.kvp["elitist"])?gaTrue:gaFalse);
        }
    } else { 
        evolver_->initGA(fromString<float>(config_.kvp["pMutation"]), fromString<int>(config_.kvp["popSize"]), fromString<bool>(config_.kvp["elitist"])?gaTrue:gaFalse);
    }
    
    // create soundcloud and twitter servers from settings file, google voice downloader, web rating downloader
    
    // add notifiers to ga
    
    // start playin'
    bounceComps();
    startPlayComp(); // will call resetTimer()
    switchState(GENERATION_START);
}

void nynexApp::Config::setFromStream(istream & is) {
    std::string line;
    while (!is.eof()) {
        is >> line;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            kvp[line.substr(0, pos-1)] = line.substr(pos-1);
        }
    }
}

//--------------------------------------------------------------
void nynexApp::update() {
    switch (state_) {
        case GENERATION_START:
            if (!player_.getIsPlaying()) {
                if (moreComps()) {
                    playNextComp();
                } else {
                    switchState(GENERATION_LIST);
                }
            }
            break;
        case GENERATION_LIST:
            if (timesUp()) {
                if (gotRatings()) {
                    evolver_->stepGA();
                    bounceComps();
                }
                startPlayComp();
                switchState(GENERATION_START);
            }
            break;
        case GENERATION_RATE:
            if (timesUp()) {
                switchState(GENERATION_LIST);
            }
            break;
        case GENERATION_END:
            break;
        default:
            switchState(GENERATION_START); // can't happen!
    }
}

//--------------------------------------------------------------
void nynexApp::draw(){
    switch (state_) {
        case GENERATION_START:
            drawGenStart();
            break;
        case GENERATION_END:
//            drawGenEnd();
            break;
        case GENERATION_LIST:
//            drawGenList();
            break;
        case GENERATION_RATE:
//            drawGenRate();
            break;
        default:
            // can't happen
            switchState(GENERATION_START);
    }
    ++framesSinceStateChange_;
}

//--------------------------------------------------------------
void nynexApp::keyPressed(int key){

}

//--------------------------------------------------------------
void nynexApp::keyReleased(int key){

}

//--------------------------------------------------------------
void nynexApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void nynexApp::mouseDragged(int x, int y, int button){
    // if button == left mouse
    // if position within a button, change activebutton
}

//--------------------------------------------------------------
void nynexApp::mousePressed(int x, int y, int button){
    // if button == left mouse
    // if position within a button, change activebutton
}

//--------------------------------------------------------------
void nynexApp::mouseReleased(int x, int y, int button){
    // check position against all buttons
}

//--------------------------------------------------------------
void nynexApp::windowResized(int w, int h){
    ofSetFullscreen(true); // don't fuck with the window, jerkface
}


void nynexApp::startPlayComp() {
    compIndex_ = -1;
    playNextComp();
}

void nynexApp::playNextComp() {
    ++compIndex_;
    player_.loadSound(bouncepath_+"/"+fileForGenAndIndividual(evolver_->getGA().generation(), compIndex_));
}

bool nynexApp::moreComps() {
    return (compIndex_ < evolver_->getPop().size());
}

void nynexApp::bounceComps() {
    for(size_t i = 0; i < evolver_->getPop().size(); ++i) {
        dynamic_cast<const Composition &>(evolver_->getPop().individual(i)).bounceToFile(bouncepath_+"/"+fileForGenAndIndividual(evolver_->getGA().generation(), i));
    }
}

bool nynexApp::gotRatings() {
//    Ratings::getInstance().getServerRatings();
    for (size_t i = 0; i < evolver_->getPop().size(); ++i) {
        if (Ratings::getInstance().hasRatingForId(i)) {
            return true;
        }
    }
    return false;
}

void nynexApp::drawGenStart() {
}