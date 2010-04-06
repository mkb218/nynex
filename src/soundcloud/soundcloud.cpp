/*
 *  soundcloud.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 1/28/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include "soundcloud.h"
#include "app.h"
#include <iostream>
#include <fstream>
#include <stdio.h>
#include <boost/foreach.hpp>
#undef check
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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

SoundCloudServer::SoundCloudServer(const std::string & key, const std::string & secret, const std::string & filepath, const std::string & scpcmd, bool useSandbox) : filepath_(filepath), scpcmd_(scpcmd), authenticated_(false) { scApi_ = SoundCloudCAPI_CreateWithDefaultCallbackAndGetCredentials(key.c_str(), secret.c_str(), "", !useSandbox); }


SoundCloudServer::~SoundCloudServer() {
    if (scApi_ != NULL) {
        SoundCloudCAPI_Delete(scApi_);
    }
}

/*void SoundCloudServer::fetchDropBox() const {
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
*/


std::vector<std::string> SoundCloudServer::submitCompositions(const GAGeneticAlgorithm & ga) const {
    int gen = ga.generation();
    std::vector<std::string> ids;
    for (int i = 0; i < ga.population().size(); ++i) {
        Composition & comp = dynamic_cast<Composition &>(ga.population().individual(i));
        // punt to id3v2 to set tags
        std::string cmd("id3v2 -a 'Republic of Nynex' -A 'Generation ");
        std::string genstr(stringFrom(gen));
        std::string filestr(filepath_);
        std::string namestr("Generation ");
        namestr.append(genstr).append(" Individual ").append(stringFrom(i));
        filestr.append("/").append(fileForGenAndIndividual(gen,i));
        cmd.append(genstr).append("' -T '").append(namestr).append("' -c 'http://nynex.hydrogenproject.com' '").append(filestr).append("'");
        
        // upload track to soundcloud
        if (!authenticated_) {
            authenticate();
        }
        
        SoundCloudCAPI_SetResponseFormat(scApi_, SCResponseFormatJSON);
        // Upload an mp3:
        FILE *f=fopen(filestr.c_str(), "r");
        fseek(f,0,SEEK_END);
        int datalen=ftell(f);
        fseek(f,0,SEEK_SET);
        char *data=(char*)malloc(datalen);
        fread(data,1,datalen,f);
        fclose(f);
        
        SoundCloudCAPI_Parameter params[2];
        params[0].key="track[title]";
        params[0].value=namestr.c_str();
        params[1].key="track[asset_data]";
        params[1].value=data;
        params[1].value_len=datalen;
        params[1].filename=filestr.c_str();
        int err; void * rcvdata; unsigned long long size;
        SoundCloudCAPI_performMethod(scApi_,"POST","/me/tracks",params,2, &err, &rcvdata, &size);
        free(data);

        using boost::property_tree::ptree;
        ptree pt;
        istringstream is;
        is.str(std::string((char*)data, size));

        // keep ID handy
        ids.push_back(pt.get<std::string>(ptree::path_type("id")));
    }
}

