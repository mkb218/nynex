#ifndef _NYNEX_APP
#define _NYNEX_APP

#include "ofMain.h"
#include "evolver.h"
#include "soundcloud.h"
#include "twitter.h"

#define POPSIZE 10
#define GEN_LIMIT_MILLIS 3600 * 1000
#define RATINGS 5
#define RATEBUTTON_RADIUS 100
#define PLAYBUTTON_RADIUS 100
#define ACTIVE_RADIUS_INCREMENT 50

#define TRANSITION_FRAMES 60
#define FONTFILE_PATH "/Library/Fonts/Futura.dfont"

#define LR_MARGIN 100
#define VERT_MARGIN 100

#define START_R 0
#define END_R 255
#define R_SLOPE ((END_R - START_R) / (float)POPSIZE)
#define START_G 0
#define END_G 255
#define G_SLOPE ((END_G - START_G) / (float)POPSIZE)
#define START_B 255
#define END_B 0
#define B_SLOPE ((END_B - START_B) / (float)POPSIZE)

namespace nynex {
class nynexApp : public ofBaseApp{

public:
    nynexApp() : mode_(STARTUP), sc_(NULL), evolver_(NULL), twitter_(NULL) {
        ofBaseApp();
    }
    ~nynexApp() { 
        delete sc_;
        delete evolver_;
        delete twitter_;
    }
    void setup();
    void update();
    void draw();

    void keyPressed  (int key);
    void keyReleased(int key);
    void mouseMoved(int x, int y );
    void mouseDragged(int x, int y, int button);
    void mousePressed(int x, int y, int button);
    void mouseReleased(int x, int y, int button);
    void windowResized(int w, int h);
private:
    enum Mode { STARTUP,
        GENERATION_START,
        GENERATION_WAIT } mode_;
    struct Button {
        float x;
        float y;
        struct {
            int r;
            int g;
            int b;
        } color;
        float radius;
    }
        
    void resetTimer() { timer_ = ofGetElapsedTimeMillis(); }
    bool timesUp() { return (ofGetElapsedTimeMillis() - timer_) > GEN_LIMIT_MILLIS; }
    void resetFrames() { framesSinceStateChange_ = 0; }
    void bounceComps();
    void startPlayComp();
    void playNextComp();
    bool gotRatings();
    bool moreComps();
    void drawStartup();
    void drawGenEnd();
    void drawGenStart();
    int timer_;
    unsigned int framesSinceStateChange_;
    Evolver * evolver_;
    SoundCloudServer * sc_;
    TwitterServer * twitter_;
    ofSoundPlayer player;
    size_t compIndex_;
    Button[POPSIZE] playButtons;
    Button[RATINGS] rateButtons;
};
}
#endif
