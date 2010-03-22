#ifndef _NYNEX_APP
#define _NYNEX_APP

#include "ofMain.h"
#include "evolver.h"
#include "soundcloud.h"
#include "twitter.h"

#include <string>

#define POPSIZE 10
#define GEN_LIMIT_MILLIS 3600 * 1000
#define RATE_LIMIT_MILLIS 60 * 1000 * 5
#define RATINGS 5
#define RATEBUTTON_RADIUS 100
#define PLAYBUTTON_RADIUS 100
#define ACTIVE_RADIUS_INCREMENT 50
#define HOVER_RADIUS_INCREMENT 50

#define TRANSITION_FRAMES 60
#define FONTFILE_PATH "/Library/Fonts/Futura.dfont"

#define LR_MARGIN 100
#define VERT_MARGIN 100

#define BG_R 255
#define BG_G 255
#define BG_B 255
#define START_R 0
#define END_R 128
#define R_SLOPE ((END_R - START_R) / (float)POPSIZE)
#define START_G 64
#define END_G 0
#define G_SLOPE ((END_G - START_G) / (float)POPSIZE)
#define START_B 0
#define END_B 0
#define B_SLOPE ((END_B - START_B) / (float)POPSIZE)

namespace nynex {
class nynexApp : public ofBaseApp{

public:
    nynexApp() : state_(GENERATION_START), sc_(NULL), evolver_(NULL), twitter_(NULL), activeButton_(NULL), samplepath_("/opt/nynex/samples"), bouncepath_("/opt/nynex/output") {
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
    enum State { GENERATION_START,
        GENERATION_LIST,
        GENERATION_RATE,
        GENERATION_END
    } state_;
    struct Button {
        float x;
        float y;
        struct {
            int r;
            int g;
            int b;
        } color;
        float radius;
    };
        
    void resetTimer() { timer_ = ofGetElapsedTimeMillis(); }
    bool timesUp() { return (ofGetElapsedTimeMillis() - timer_) > GEN_LIMIT_MILLIS; }
    void switchState(State state) { state_ = state; framesSinceStateChange_ = 0; }
    void bounceComps();
    void startPlayComp();
    void playNextComp();
    bool gotRatings();
    bool moreComps();
    void drawGenEnd();
    void drawGenStart();
    void drawGenList();
    void drawGenRate();
    int timer_;
    unsigned int framesSinceStateChange_;
    Evolver * evolver_;
    SoundCloudServer * sc_;
    TwitterServer * twitter_;
    ofSoundPlayer player_;
    int compIndex_;
    std::string samplepath_;
    std::string bouncepath_;
    Button *activeButton_;
    Button playButtons_[POPSIZE];
    Button rateButtons_[RATINGS];
};
}
#endif
