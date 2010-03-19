#include "app.h"

using namespace nynex;

// app has following states
// 0.) init ga, bounce all comps to files and play
// 1.) we play all compositions in sequence, timer starts. once playing is done, commence step 2. display running countdown.
// 2.) display buttons for each composition allowing users to listen. display running countdown
// 3.) if user clicks listen button, display rating buttons plus cancel button. rating times out after 5 minutes
// 4a.) user clicks rating button, rating stored, back to step 2
// 4b.) user clicks cancel button, no rating stored, back to step 2
// 5.) time's up, retrieve web ratings. if there are ratings, step ga and bounce, otherwise just go back to step 1



//--------------------------------------------------------------
void nynexApp::setup(){
    // fullscreen
    ofSetFullscreen(true);
    
    // set background color
    ofSetBackground(BG_R, BG_G, BG_B);
    ofSetFrameRate(60);
    
    // set frame rate + vert refresh
    // init ga (from file if exists)
    // create soundcloud and twitter servers from settings file, google voice downloader, web rating downloader
    // add notifiers to ga
    bounceComps();
    startPlayComp(); // will call resetTimer()
    switchState(GENERATION_START);
}

//--------------------------------------------------------------
void nynexApp::update() {
    switch (mode_) {
        case GENERATION_START:
            if (!player.getIsPlaying()) {
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

