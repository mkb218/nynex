#include "app.h"
#include "ratings.h"

#define SANDBOX 0

#include <iostream>
#include <sys/stat.h>

#include "gvoice.h"
#include "uploads.h"

using namespace nynex;

// app has following states
// 0.) init ga, bounce all comps to files and play
// 1.) we play all compositions in sequence, timer starts. once playing is done, commence step 2. display running countdown.
// 2.) display buttons for each composition allowing users to listen. display running countdown
// 3.) if user clicks listen button, display rating buttons plus cancel button. rating times out after 5 minutes
// 4a.) user clicks rating button, rating stored, back to step 2
// 4b.) user clicks cancel button, no rating stored, back to step 2
// 5.) time's up, retrieve web ratings. if there are ratings, step ga and bounce, otherwise just go back to step 1

void BounceAction::action(const GAGeneticAlgorithm & ga) {
    app_->bounceComps();
    while (app_->bounceThread_->isThreadRunning()) {
        sleep(1);
    }
}

std::string nynex::fileForGenAndIndividual(int gen, int i) {
    return std::string("gen")+stringFrom(gen)+"i"+stringFrom(i)+".aiff";
}

void BounceThread::threadedFunction() {
    bool done;
    {
        Gatekeeper g(&listmutex_);
        done = bounces_.empty();
    }
    
    while (!done) {
        std::cout << "bouncing to file " << bounces_.front().second << std::endl;
        bounces_.front().first->bounceToFile(bounces_.front().second);
        {
            Gatekeeper g(&listmutex_);
            bounces_.pop_front();
            done = bounces_.empty();
        }
    }
    std::cout << "thread complete" << std::endl;
}
nynexApp::~nynexApp() { 
    delete sc_;
    evolver_->saveToFile(config_.kvp["gastatefile"]);
    Ratings::getInstance().saveToFile(config_.kvp["ratingsfile"]);
    delete evolver_;
    delete twitter_;
}

//--------------------------------------------------------------
void nynexApp::setup(){
    // fullscreen
    ofSetFullscreen(false);
    
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
    SampleBank::getInstance().setTmpDir(config_.kvp["tmpdir"]);
    SampleBank::getInstance().setSampleDir(samplepath_);
    
    // init ga (from file if exists)
    evolver_ = new Evolver();
    struct stat junk;
    bool cached = false;
    if (0 == stat(config_.kvp["gastatefile"].c_str(), &junk)) {
        try {
            evolver_->loadFromFile(config_.kvp["gastatefile"]);
            cached = true;
        } catch ( ... ) {
            evolver_->initGA(fromString<float>(config_.kvp["pMutation"]), fromString<int>(config_.kvp["popSize"]), fromString<bool>(config_.kvp["elitist"])?gaTrue:gaFalse);
        }
    } else { 
        evolver_->initGA(fromString<float>(config_.kvp["pMutation"]), fromString<int>(config_.kvp["popSize"]), fromString<bool>(config_.kvp["elitist"])?gaTrue:gaFalse);
    }
    
    Ratings::getInstance().setTmpPath(config_.kvp["tmpdir"]);
    Ratings::getInstance().setScpCmd(config_.kvp["ratingscp"]);
    if (0 == stat(config_.kvp["ratingsfile"].c_str(), &junk)) {
        try {
            Ratings::getInstance().loadFromFile(config_.kvp["ratingsfile"]);
        } catch ( ... ) {
            // oh well
        }
    }
    
    evolver_->addNotifier(false, new BounceAction(this));

    bool offline = fromString<bool>(config_.kvp["offline"]);
    SubmitSoundCloudAction *ssca = NULL;
    if (!offline) {
        // create soundcloud and twitter servers from settings file, google voice downloader, web rating downloader
        twitter_ = new TwitterServer(config_.kvp["twitterhost"],config_.kvp["twitteruser"],config_.kvp["twitterpass"],config_.kvp["bitlykeyfile"]);
        sc_ = new SoundCloudServer(config_.kvp["soundcloudConsumerKey"], config_.kvp["soundcloudConsumerSecret"], bouncepath_, config_.kvp["soundcloudScpCmd"], SANDBOX);

        // add notifiers to ga
        ssca = new SubmitSoundCloudAction(*sc_);
        evolver_->addNotifier(true, new GVoiceAction());
        evolver_->addNotifier(true, new GrabUploadsAction(config_.kvp["uploadUser"], config_.kvp["uploadServer"], config_.kvp["uploadPath"]));
        evolver_->addNotifier(false, ssca);
        evolver_->addNotifier(false, new TwitterAnnounce(*twitter_));
    }
    
    // setup fonts
    bigfont_.loadFont("/opt/nynex/bin/BetecknaLowerCase.ttf", BIGFONTSIZE);
    smallfont_.loadFont("/opt/nynex/bin/BetecknaLowerCase.ttf", SMALLFONTSIZE);
    
    mkdir_or_throw("/tmp/nynex");
    
    // start playin'
    if (!cached) {
        bounceComps();
        while (bounceThread_->isThreadRunning()) {
            sleep(1);
        }
        
        if (!offline) {
            ssca->action(evolver_->getGA());
        }
        startPlayComp();
    } else {
        setupListButtons();
        resetGenTimer();
        switchState(GENERATION_LIST);
    }
}

void nynexApp::Config::setFromStream(istream & is) {
    std::string line;
    while (!is.eof()) {
        getline(is,line);
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            kvp[line.substr(0, pos)] = line.substr(pos+1);
        }
    }
}

//--------------------------------------------------------------
void nynexApp::update() {
    switch (state_) {
        case GENERATION_START:
            if (!player_.getIsPlaying()) {
                if (moreComps()) {
//                    bounceComp(compIndex_);
                    playNextComp();
                } else {
                    resetGenTimer();
                    setupListButtons();
                    switchState(GENERATION_LIST);
                }
            }
            break;
        case GENERATION_LIST:
            if (generationTimesUp()) {
                switchState(GENERATION_END);
            }
            break;
        case GENERATION_RATE:
            if (ratingTimesUp()) {
                switchState(GENERATION_LIST);
            }
            break;
        case GENERATION_END:
            if (gotRatings()) {
                evolver_->stepGA();
                Ratings::getInstance().deleteRatings();
                evolver_->saveToFile(config_.kvp["gastatefile"]);
//                bounceComps(); done with BounceAction
            }
        case INIT:
            switchState(GENERATION_START);
            startPlayComp();
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
        case INIT:
            drawGenEnd();
            break;
        case GENERATION_LIST:
            drawGenList();
            break;
        case GENERATION_RATE:
            drawGenRate();
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
    checkActiveButton(x,y,0);
}

bool nynexApp::checkActiveButton(int x, int y, int button) {
    if (activeButton_ != NULL) {
        int xdist = x - activeButton_->x;
        int ydist = y - activeButton_->y;
        if (xdist*xdist + ydist*ydist > activeButton_->radius*activeButton_->radius) {
            // not active
            activeButton_->radius = (state_ == GENERATION_LIST)?listRadius_:rateRadius_;
            activeButton_ = NULL;
        } else {
            return true;
        }
    }
        
    if (state_ == GENERATION_LIST) {
        for (size_t i = 0; i < POPSIZE; ++i) {
            int xdist = x - listButtons_[i].x;
            int ydist = y - listButtons_[i].y;
            if (xdist*xdist + ydist*ydist <= listButtons_[i].radius*listButtons_[i].radius) {
                // active
                listButtons_[i].radius = listRadius_+ACTIVE_RADIUS_INCREMENT;
                activeButton_ = listButtons_+i;
                return true;
            }
        }
    } else if (state_ == GENERATION_RATE) {
        for (size_t i = 0; i <= RATINGS; ++i) {
            int xdist = x - rateButtons_[i].x;
            int ydist = y - rateButtons_[i].y;
            if (xdist*xdist + ydist*ydist <= rateButtons_[i].radius*rateButtons_[i].radius) {
                // active
                
                rateButtons_[i].radius = rateRadius_+ACTIVE_RADIUS_INCREMENT;
                activeButton_ = rateButtons_+i;
                return true;
            }
        }
    }

    return false;
}

//--------------------------------------------------------------
void nynexApp::mouseDragged(int x, int y, int button){
    // if button == left mouse
    // if position within a button, change activebutton
//    checkActiveButton(x,y,button);
}

//--------------------------------------------------------------
void nynexApp::mousePressed(int x, int y, int button){
    // if button == left mouse
    // if position within a button, change activebutton
//    checkActiveButton(x,y,button);
}



//--------------------------------------------------------------
void nynexApp::mouseReleased(int x, int y, int button){
    // check position against all buttons
    if (checkActiveButton(x,y,button)) {
        if (state_ == GENERATION_LIST) {
            compIndex_ = activeButton_ - listButtons_;
            playNextComp();
            resetRateTimer();
            setupRateButtons();
            switchState(GENERATION_RATE);
        } else if (state_ == GENERATION_RATE) {
            if (activeButton_ != rateButtons_) { // cancel
                Ratings::getInstance().addRating(dynamic_cast<Composition &>(evolver_->getPop().individual(compIndex_-1)).getObjectId(), activeButton_ - rateButtons_);
            }
            setupListButtons();
            player_.stop();
            switchState(GENERATION_LIST);
        }
    }
}

//--------------------------------------------------------------
void nynexApp::windowResized(int w, int h){
//    ofSetFullscreen(true); // don't fuck with the window, jerkface
}


void nynexApp::startPlayComp() {
    compIndex_ = 0;
}

void nynexApp::playNextComp() {
    player_.loadSound(bouncepath_+"/"+fileForGenAndIndividual(evolver_->getGA().generation(), compIndex_));
    player_.play();
    ++compIndex_;
}

bool nynexApp::moreComps() {
    return (compIndex_ < evolver_->getPop().size());
}

void nynexApp::bounceComps() {
    for(size_t i = 0; i < evolver_->getPop().size(); ++i) {
        bounceComp(i);
    }
}

void nynexApp::bounceComp(size_t i) {
    if (bounceThread_ == NULL || !bounceThread_->isThreadRunning()) {
        delete bounceThread_;
        bounceThread_ = new BounceThread(dynamic_cast<const Composition &>(evolver_->getPop().individual(i)),bouncepath_+"/"+fileForGenAndIndividual(evolver_->getGA().generation(), i));
        bounceThread_->startThread();
    } else {
        bounceThread_->addPair(dynamic_cast<const Composition &>(evolver_->getPop().individual(i)),bouncepath_+"/"+fileForGenAndIndividual(evolver_->getGA().generation(), i));
    }
}

bool nynexApp::gotRatings() {
//    Ratings::getInstance().getServerRatings();
    for (size_t i = 0; i < evolver_->getPop().size(); ++i) {
        if (Ratings::getInstance().hasRatingForId(dynamic_cast<Composition&>(evolver_->getPop().individual(i)).getObjectId())) {
            return true;
        }
    }
    return false;
}

void nynexApp::drawGenStart() {
    std::string s("Generation ");
    s = s + stringFrom(evolver_->getGA().generation()) +" Individual " + stringFrom(std::max<int>(0,compIndex_-1));
    float width = bigfont_.stringWidth(s);
    float height = bigfont_.getLineHeight();
    float hpos = (ofGetWidth() - width) / 2;
    float vpos = (ofGetHeight() - height) / 2;
    ofSetColor(0x000000);
    bigfont_.drawString(s, hpos, vpos);
}

void nynexApp::drawGenEnd() {
    drawHeader(std::string("Creating Next Generation\nContribute! Call " PHONE " or visit\nhttp://nynex.hydrogenproject.com"));
    drawEndTimer();
}

void nynexApp::drawGenList() {
    drawHeader(std::string("Please rate Generation ") + stringFrom(evolver_->getGA().generation()));

    for (size_t i = 0; i < POPSIZE; ++i) {  
        // draw button from playButtons
        Button *b = listButtons_+i;
        ofSetColor(b->r, b->g, b->b);
        ofCircle(b->x,b->y,b->radius);
        
        // draw label
        std::string label("Individual ");
        label.append(stringFrom(i));
        float x = b->x + b->radius + LR_MARGIN;
        float y = b->y + bigfont_.stringHeight(label.c_str())/2;
        ofSetColor(0);
        bigfont_.drawString(label, x, y);
    }
    
    drawGenTimer();
}

void nynexApp::drawGenRate() {
    drawHeader(std::string("Rating Generation ") + stringFrom(evolver_->getGA().generation()) +" Individual " + stringFrom(compIndex_-1));
    for (size_t i = 0; i < RATINGS+1; ++i) {  
        // draw button from playButtons
        Button *b = rateButtons_+i;
        ofSetColor(b->r, b->g, b->b);
        ofCircle(b->x,b->y,b->radius);
        
        // draw label
        std::string label(!i?"Cancel":stringFrom(i));
        float x = b->x + b->radius + LR_MARGIN;
        float y = b->y + bigfont_.stringHeight(label.c_str())/2;
        ofSetColor(0);
        bigfont_.drawString(label, x, y);
    }

    drawRateTimer();
}

void nynexApp::drawHeader(std::string s) {
    float width = bigfont_.stringWidth(s);
    float height = bigfont_.getLineHeight();
    float pos = (ofGetWidth() - width) / 2;
    ofSetColor(0x000000);
    bigfont_.drawString(s, pos, height+VERT_MARGIN);
}

void nynexApp::drawRateTimer() {
    drawTimer(RATE_LIMIT_MILLIS - (ofGetElapsedTimeMillis() - ratetimer_));
}

void nynexApp::drawGenTimer() {
    drawTimer(GEN_LIMIT_MILLIS - (ofGetElapsedTimeMillis() - gentimer_));
}

void nynexApp::drawEndTimer() {
    drawTimer(framesSinceStateChange_ * -60);
}

void nynexApp::drawTimer(int timeleft) {
    if (timeleft > 0) {
        ofSetColor(0x000000);
    } else {
        timeleft = -timeleft;
        ofSetColor(TIMER_RED);
    }
    
    int millis = timeleft % 1000;
    timeleft /= 1000;
    int seconds = timeleft % 60;
    timeleft /= 60;
    int minutes = timeleft % 60;
    timeleft /= 60;
    int hours = timeleft % 60;
    
    std::ostringstream os;
    os << hours;
    os << ":" << std::setw(2) << std::setfill('0');
    os<< minutes;
    os << ":" << std::setw(2) << std::setfill('0');
    os << seconds;
    os << "." << std::setw(2) << std::setfill('0');
    os << millis;
    std::string time = os.str();
    float width = bigfont_.stringWidth(time);
    float pos = (ofGetWidth() - width) / 2;
    bigfont_.drawString(time, pos, ofGetHeight() - VERT_MARGIN);
}

void nynexApp::setupListButtons() {
    activeButton_ = NULL;
    float radius = ofGetHeight();
    radius -= (bigfont_.getLineHeight() * 2); // header and timer
    radius /= (POPSIZE/2); // rows
    radius -= VERT_MARGIN * 2;
    radius /= 2;
    listRadius_ = radius;
    for (size_t i = 0; i < POPSIZE; ++i) {  
        // draw button from playButtons
        listButtons_[i].r = START_R + R_SLOPE * i;
        listButtons_[i].g = START_G + G_SLOPE * i;
        listButtons_[i].b = START_B + B_SLOPE * i;
        listButtons_[i].radius = radius;

        float x = LR_MARGIN + (radius);
        if (i >= POPSIZE / 2) {
            x += ofGetWidth() / 2 ;
        }
        
        listButtons_[i].x = x;
        
        float y = radius + (bigfont_.getLineHeight() + VERT_MARGIN); // past header
        y += ((radius * 2 + VERT_MARGIN) * (i % (POPSIZE/2))); // past existing circles
        y += radius;
        listButtons_[i].y = y;
    }
}

void nynexApp::setupRateButtons() {
    activeButton_ = NULL;
    float radius = ofGetHeight();
    radius -= (bigfont_.getLineHeight() * 2); // header and timer
    radius /= (RATINGS+1); // rows
    radius -= VERT_MARGIN * 2;
    radius /= 2;
    rateRadius_ = radius;
    
    float x = (ofGetWidth() - LR_MARGIN - bigfont_.stringWidth("Cancel") - radius) / 2;
    for (size_t i = 0; i <= RATINGS; ++i) {  
        // draw button from playButtons
        rateButtons_[i].r = START_R + R_SLOPE * i;
        rateButtons_[i].g = START_G + G_SLOPE * i;
        rateButtons_[i].b = START_B + B_SLOPE * i;
        rateButtons_[i].radius = radius;
        
        rateButtons_[i].x = x;
        
        float y = radius + (bigfont_.getLineHeight() + VERT_MARGIN); // past header
        y += ((radius * 2 + VERT_MARGIN) * (i % (RATINGS+1))); // past existing circles
        y += radius;
        rateButtons_[i].y = y;
    }
}

void nynexApp::switchState(State state) { 
    if (bounceThread_ == NULL || !bounceThread_->isThreadRunning()) {
        state_ = state;
        framesSinceStateChange_ = 0; 
    }
}
