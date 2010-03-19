#include "app.h"

using namespace nynex;

// app has three modes, passed in sequence
// 0.) init ga, bounce all comps to files and play
// 1.) we play all compositions in sequence, timer starts
// 2.) display buttons for each composition allowing users to listen and rate. display running countdown
// 3.) time's up, retrieve web ratings. if there are ratings, step ga and bounce, otherwise just go back to mode 1



//--------------------------------------------------------------
void nynexApp::setup(){
    // fullscreen
    ofSetFullscreen(true);
    
    // set background color
    ofSetFrameRate(60);
    
    // set frame rate + vert refresh
    // init ga (from file if exists)
    // create soundcloud and twitter servers from settings file, google voice downloader, web rating downloader
    // add notifiers to ga
}

//--------------------------------------------------------------
void nynexApp::update() {
    switch (mode_) {
        case STARTUP:
            bounceComps();
            startPlayComp(); // will call resetTimer()
            mode_ = GENERATION_START;
            resetFrames();
            break;
        case GENERATION_START:
            if (!player.getIsPlaying()) {
                if (moreComps()) {
                    playNextComp();
                } else {
                    mode_ = GENERATION_WAIT;
                    resetFrames();
                }
            }
            break;
        case GENERATION_WAIT:
            if (timesUp()) {
                if (gotRatings()) {
                    evolver_->stepGA();
                    bounceComps();
                }
                startPlayComp();
                resetFrames();
                mode_ = GENERATION_START;
            }
            break;
        default:
            mode_ = GENERATION_START; // can't happen!
            resetFrames();
    }
}

//--------------------------------------------------------------
void nynexApp::draw(){
    switch (mode_) {
        case STARTUP:
            drawStartup();
            break;
        case GENERATION_START:
            drawGenStart();
            break;
        case GENERATION_END:
            drawGenEnd();
            break;
        default:
            // can't happen
            mode_ = GENERATION_START;
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
    // 
}

//--------------------------------------------------------------
void nynexApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void nynexApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void nynexApp::windowResized(int w, int h){

}

