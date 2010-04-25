#ifndef _NYNEX_APP
#define _NYNEX_APP

#include "ofMain.h"
#include "ofxThread.h"
#include "evolver.h"
#include "soundcloud.h"
#include "twitter.h"

#include <string>
#include <list>
#include <fstream>

#define POPSIZE 10
#define GEN_LIMIT_MILLIS 36000
#define RATE_LIMIT_MILLIS 60 * 1000 * 5
#define RATINGS 5

#define ACTIVE_RADIUS_INCREMENT 10
#define HOVER_RADIUS_INCREMENT 20

#define BIGFONTSIZE 32
#define SMALLFONTSIZE 18

#define PHONE "(978) 406-9639"

#define TRANSITION_FRAMES 60

#define LR_MARGIN 25
#define VERT_MARGIN 35

#define TIMER_RED 0xff0000
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

class Gatekeeper {
public:
    Gatekeeper(pthread_mutex_t *l) : l_(l) { pthread_mutex_lock(l_); }
    ~Gatekeeper() { pthread_mutex_unlock(l_); }
private:
    pthread_mutex_t *l_;
};
    
class nynexApp;
    
class BounceThread : public ofxThread {
public:
    BounceThread(const Composition & c, const std::string & s) : ofxThread() { 
        pthread_mutex_init(&listmutex_, NULL);
        addPair(c, s); 
    }
    virtual ~BounceThread() {
        pthread_mutex_destroy(&listmutex_);
    }
    void addPair(const Composition & c, const std::string & s) {
        Gatekeeper g(&listmutex_);
        std::cout << "adding file " << s<< std::endl;
        bounces_.push_back(std::make_pair(&c, s));
    }
    virtual void threadedFunction();
private:
    pthread_mutex_t listmutex_;
    std::list<std::pair<const Composition *, std::string> > bounces_;
};

class BounceAction : public StepAction {
public:
    BounceAction(nynexApp * n) : app_(n){}
    virtual void action(const GAGeneticAlgorithm & ga);
private:
    nynexApp *app_;
};
    
class nynexApp : public ofBaseApp{

public:
    nynexApp() : state_(INIT), sc_(NULL), evolver_(NULL), twitter_(NULL), activeButton_(NULL), samplepath_("/opt/nynex/samples"), bouncepath_("/opt/nynex/output"), configpath_("/opt/nynex/etc/nynex.conf"), bounceThread_(NULL), framesSinceStateChange_(0) {
        ofBaseApp();
    }
    ~nynexApp() { 
        delete sc_;
        evolver_->saveToFile(config_.kvp["gastatefile"]);
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
    enum State { INIT,
        GENERATION_START,
        GENERATION_LIST,
        GENERATION_RATE,
        GENERATION_END
    } state_;
    
    struct Config {
        Config() {
            // defaults!
            kvp["gastatefile"] = "/opt/nynex/var/gastate";
            kvp["pMutation"] = "0.1";
            kvp["popSize"] = "10";
            kvp["elitist"] = "1";
            kvp["samplerate"] = "44100";
            kvp["samplesize"] = "2";
            kvp["channels"] = "2";
//            kvp["twitteruser"] = "nynexrepublic";
            
        }
        void setFromStream(std::istream & i);
        std::map<std::string, std::string> kvp;
    };
    
    struct Button {
        float x;
        float y;
        float radius;
        float r;
        float g;
        float b;
    };
    
    void resetRateTimer() { ratetimer_ = ofGetElapsedTimeMillis(); }
    void resetGenTimer() { gentimer_ = ofGetElapsedTimeMillis(); }
    bool generationTimesUp() { return (ofGetElapsedTimeMillis() - gentimer_) > GEN_LIMIT_MILLIS; }
    bool ratingTimesUp() { return (ofGetElapsedTimeMillis() - ratetimer_) > RATE_LIMIT_MILLIS; }
    void switchState(State state);
    void bounceComps();
    void bounceComp(size_t i);
    void startPlayComp();
    void playNextComp();
    bool gotRatings();
    bool moreComps();
    void drawHeader(std::string);
    void drawTimer(int);
    void drawGenTimer();
    void drawRateTimer();
    void drawEndTimer();
    void drawGenEnd();
    void drawGenStart();
    void drawGenList();
    void drawGenRate();
    void setupListButtons();
    void setupRateButtons();
    bool checkActiveButton(int x, int y, int button);
    Config config_;
    int ratetimer_;
    int gentimer_;
    unsigned int framesSinceStateChange_;
    Evolver * evolver_;
    SoundCloudServer * sc_;
    TwitterServer * twitter_;
    ofSoundPlayer player_;
    BounceThread *bounceThread_;
    ofTrueTypeFont bigfont_;
    ofTrueTypeFont smallfont_;
    int compIndex_;
    std::string samplepath_;
    std::string bouncepath_;
    std::string configpath_;
    float listRadius_;
    float rateRadius_;
    Button listButtons_[POPSIZE];
    Button rateButtons_[RATINGS+1];
    Button * activeButton_;
    friend void BounceAction::action(const GAGeneticAlgorithm &);
};
}
#endif
