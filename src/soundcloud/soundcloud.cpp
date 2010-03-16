/*
 *  soundcloud.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 1/28/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include "soundcloud.h"
#include <iostream>
#include <fstream>
#include <boost/foreach.hpp>

using namespace nynex;

SoundCloudAuthGenerator SoundCloudServer::defaultAuthenticatorGenerator_ = TerminalAuthenticator::create;

SoundCloudAuthenticator * TerminalAuthenticator::create(SoundCloudCAPI * api) {
    return new TerminalAuthenticator(api);
}

bool TerminalAuthenticator::authenticate() {
    while (!authserver()) {
        std::string buffer;
        std::cout << "Please Auth, then enter the verification code here: " << std::flush;
        setverifier(buffer);
    }
}

void SubmitSoundCloudAction::action(const GAGeneticAlgorithm & ga) {
    std::vector<std::string> comps = server_->submitCompositions(ga);
    std::string outfilename(server_->getFilepath()+stringFrom(ga.generation()));
    std::ofstream outf(outfilename.c_str()); // todo check status
    
    // store result, upload map file
    // filename: ~/genmaps/<number>/
    // contains one soundcloud track id per line
    
    BOOST_FOREACH(std::string & c, comps) {
        outf << c << std::endl;
    }
    outf.close();
    
    std::string scpcmd(server_->getScpcmd());
    size_t pos = scpcmd.find("%f");
    scpcmd.erase(pos, 2);
    scpcmd.insert(pos, outfilename);
    system(scpcmd.c_str());
}

SoundCloudServer::~SoundCloudServer() {
    if (scApi_ != NULL) {
        SoundCloudCAPI_Delete(scApi_);
    }
}

void SoundCloudServer::fetchDropBox() const {
    void *data;
    unsigned long long datalen;
    int errnum;
    SoundCloudCAPI_Parameter filter = { "filter", "drop", 0, NULL };
    int res = SoundCloudCAPI_performMethod(scApi_, "GET", "/events", &filter, 1 ,&errnum, &data, &datalen);
    if (!res) {
        std::cout << "something wrong" << std::endl;
    }
    
    // now what?
    
    // foreach file in list
    // download file
    // delete file from dropbox
    // convert to WAV in preferred format
    // SampleBank::getInstance().addSample(WAV)
}

std::vector<std::string> SoundCloudServer::submitCompositions(const GAGeneticAlgorithm & ga) const {
    int gen = ga.generation();
    for (int i = 0; i < ga.population().size(); ++i) {
        Composition & comp = dynamic_cast<Composition &>(ga.population().individual(i));
        comp.bounceToFile(filepath_+"/gen"+stringFrom(gen) + "i" + stringFrom(i));
        // punt to id3v2 to set tags
        // remove previous generation of track?
        // upload track to soundcloud
    }
}

