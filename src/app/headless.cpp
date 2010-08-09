/*
 *  headless.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 8/9/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include "ratings.h"

#define SANDBOX 0

#include <iostream>
#include <fstream>
#include <sys/stat.h>

#include "gvoice.h"
#include "soundcloud.h"
#include "uploads.h"
#include "twitter.h"

using namespace nynex;

// headless is meant to be run once daily
// 1.) retrieve web ratings. 
// 2.) if there are ratings, step ga and bounce files, upload and notify

struct HeadlessCtx {
    ~HeadlessCtx();
    // should really extract this config Struct into one common location but who cares
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
    
    void setup();
    void bounceComps();
    void bounceComp(size_t i);
    bool gotRatings();
    bool moreComps();
    Config config_;
    Evolver * evolver_;
    SoundCloudServer * sc_;
    TwitterServer * twitter_;
    std::string samplepath_;
    std::string bouncepath_;
    std::string configpath_;
};

static void setup(HeadlessCtx *ctx, const char *configpath);

extern "C" int main(int argc, char *argv[]) {
    HeadlessCtx ctx;
    if (argc > 1) {
        setup(&ctx, argv[0]);
    } else {
        setup(&ctx, NULL);
    }
    if (ctx.gotRatings()) {
        ctx.evolver_->stepGA();
        Ratings::getInstance().deleteRatings();
        ctx.evolver_->saveToFile(*(ctx.statefile_)); // TODO find statefile
    }
    return 0;
}

class BounceAction : public StepAction {
public:
    BounceAction(HeadlessCtx * n) : app_(n){}
    virtual void action(const GAGeneticAlgorithm & ga);
private:
    HeadlessCtx *app_;
};

void BounceAction::action(const GAGeneticAlgorithm & ga) {
    app_->bounceComps();
}

HeadlessCtx::~HeadlessCtx() { 
    delete sc_;
    evolver_->saveToFile(config_.kvp["gastatefile"]);
    Ratings::getInstance().saveToFile(config_.kvp["ratingsfile"]);
    delete evolver_;
    delete twitter_;
}

//--------------------------------------------------------------
void setup(HeadlessCtx *ctx, const char *configpath){
    // init config if file exists
    if (configpath != NULL) {
        std::ifstream is(configpath); // TODO: grab from cmdline
        if (!is.fail()) {
            ctx->config_.setFromStream(is);
        }
    }
    
    // must get this from config!
    SampleBank::getInstance().setSampleRate(fromString<double>(ctx->config_.kvp["samplerate"]));
    SampleBank::getInstance().setSampleSize(fromString<int>(ctx->config_.kvp["samplesize"]));
    SampleBank::getInstance().setChannels(fromString<int>(ctx->config_.kvp["channels"]));
    SampleBank::getInstance().setTmpDir(ctx->config_.kvp["tmpdir"]);
    SampleBank::getInstance().setSampleDir(ctx->samplepath_);
    
    // init ga (from file if exists)
    ctx->evolver_ = new Evolver();
    struct stat junk;
    bool cached = false;
    if (0 == stat(ctx->config_.kvp["gastatefile"].c_str(), &junk)) {
        try {
            ctx->evolver_->loadFromFile(ctx->config_.kvp["gastatefile"]);
            cached = true;
        } catch ( ... ) {
            ctx->evolver_->initGA(fromString<float>(ctx->config_.kvp["pMutation"]), fromString<int>(ctx->config_.kvp["popSize"]), fromString<bool>(ctx->config_.kvp["elitist"])?gaTrue:gaFalse);
        }
    } else { 
        ctx->evolver_->initGA(fromString<float>(ctx->config_.kvp["pMutation"]), fromString<int>(ctx->config_.kvp["popSize"]), fromString<bool>(ctx->config_.kvp["elitist"])?gaTrue:gaFalse);
    }
    
    Ratings::getInstance().setTmpPath(ctx->config_.kvp["tmpdir"]);
    Ratings::getInstance().setScpCmd(ctx->config_.kvp["ratingscp"]);
    if (0 == stat(ctx->config_.kvp["ratingsfile"].c_str(), &junk)) {
        try {
            Ratings::getInstance().loadFromFile(ctx->config_.kvp["ratingsfile"]);
        } catch ( ... ) {
            // oh well
        }
    }
    
    ctx->evolver_->addNotifier(false, new BounceAction(ctx));
    
    bool offline = fromString<bool>(ctx->config_.kvp["offline"]);
    ctx->sc_ = new SoundCloudServer(ctx->config_.kvp["soundcloudConsumerKey"], ctx->config_.kvp["soundcloudConsumerSecret"], ctx->bouncepath_, ctx->config_.kvp["soundcloudScpCmd"], SANDBOX, offline);
    SubmitSoundCloudAction *ssca = new SubmitSoundCloudAction(*ctx->sc_, offline);
    
    if (!offline) {
        // create soundcloud and twitter servers from settings file, google voice downloader, web rating downloader
        ctx->twitter_ = new TwitterServer(ctx->config_.kvp["twitterhost"],ctx->config_.kvp["twitteruser"],ctx->config_.kvp["twitterpass"],ctx->config_.kvp["bitlykeyfile"]);
        
        // add notifiers to ga
        ctx->evolver_->addNotifier(true, new GVoiceAction());
        ctx->evolver_->addNotifier(true, new GrabUploadsAction(ctx->config_.kvp["uploadUser"], ctx->config_.kvp["uploadServer"], ctx->config_.kvp["uploadPath"]));
        ctx->evolver_->addNotifier(false, ssca);
        ctx->evolver_->addNotifier(false, new TwitterAnnounce(*ctx->twitter_));
    } else {
        ctx->evolver_->addNotifier(false, ssca);
    }
    
    mkdir_or_throw("/tmp/nynex");
    
}

void HeadlessCtx::Config::setFromStream(std::istream & is) {
    std::string line;
    while (!is.eof()) {
        getline(is,line);
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            kvp[line.substr(0, pos)] = line.substr(pos+1);
        }
    }
}

