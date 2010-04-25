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

SoundCloudAuthGenerator SoundCloudServer::defaultAuthenticatorGenerator_ = FileAuthenticator::create;

SoundCloudAuthenticator * TerminalAuthenticator::create(SoundCloudCAPI * api) {
    return new TerminalAuthenticator(api);
}

SoundCloudAuthenticator * FileAuthenticator::create(SoundCloudCAPI * api) {
    return new FileAuthenticator(api);
}

bool TerminalAuthenticator::authenticate() {
    while (authserver() != SCAuthenticationStatus_Authenticated) {
        std::string buffer;
        std::cout << "Please Auth, then enter the verification code here: " << std::flush;
        std::cin >> buffer;
        setverifier(buffer);
    }
}

bool FileAuthenticator::authenticate() {
    while (authserver() != SCAuthenticationStatus_Authenticated) {
        std::string buffer;
        std::cout << "Please Auth, then enter the verification code into /opt/nynex/etc/verifier within 60 seconds. " << std::endl;
        sleep(60);
        ifstream in("/opt/nynex/etc/verifier");
        getline(in, buffer);
        setverifier(buffer);
    }
    return true;
}

int SoundCloudAuthenticator::authserver() {
    // Are we authorized yet?
    return SoundCloudCAPI_EvaluateCredentials(api_);
}

void SoundCloudAuthenticator::setverifier(const std::string & buffer) {
    SoundCloudCAPI_SetVerifier(api_, buffer.c_str());
}

void SubmitSoundCloudAction::action(const GAGeneticAlgorithm & ga) {
#if 1
    std::vector<std::string> comps = server_->submitCompositions(ga);
#else
    std::vector<std::string> comps;
    comps.push_back("2096511");
    comps.push_back("2096512");
    comps.push_back("2096513");
    comps.push_back("2096514");
    comps.push_back("2096515");
    comps.push_back("2096516");
    comps.push_back("2096517");
    comps.push_back("2096518");
    comps.push_back("2096519");
    comps.push_back("2096520");
#endif
    std::string outfilename(server_->getFilepath()+"/ratings.txt");
    std::ofstream outf(outfilename.c_str()); // todo check status
    
    // store result, upload map file
    // filename: ~/genmaps/<number>/
    // contains one soundcloud track id per line
    
    outf << ga.generation() << std::endl;
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

SoundCloudServer::SoundCloudServer(const std::string & key, const std::string & secret, const std::string & filepath, const std::string & scpcmd, bool useSandbox) : filepath_(filepath), scpcmd_(scpcmd), authenticated_(false), authenticator_(NULL) { scApi_ = SoundCloudCAPI_CreateWithDefaultCallbackAndGetCredentials(key.c_str(), secret.c_str(), "", !useSandbox); }


SoundCloudServer::~SoundCloudServer() {
    delete authenticator_;
    if (scApi_ != NULL) {
        SoundCloudCAPI_Delete(scApi_);
    }
}

std::vector<std::string> SoundCloudServer::submitCompositions(const GAGeneticAlgorithm & ga) const {
    int gen = ga.generation();
    std::vector<std::string> ids;
    for (int i = 0; i < ga.population().size(); ++i) {
        Composition & comp = dynamic_cast<Composition &>(ga.population().individual(i));
        // punt to id3v2 to set tags
        std::cout << " uploading track " << i << std::endl;
        std::string cmd("id3v2 -a 'Republic of Nynex' -A 'Generation ");
        std::string genstr(stringFrom(gen));
        std::string filestr(filepath_);
        std::string namestr("Generation ");
        namestr.append(genstr).append(" Individual ").append(stringFrom(i));
        filestr.append("/").append(fileForGenAndIndividual(gen,i));
        cmd.append(genstr).append("' -t '").append(namestr).append("' -c ':http://nynex.hydrogenproject.com' '").append(filestr).append("'");
        system(cmd.c_str());
        
        // upload track to soundcloud
        while (!authenticated_) {
            authenticate();
        }
        
        SoundCloudCAPI_SetResponseFormat(scApi_, SCResponseFormatJSON);
        FILE *f=fopen(filestr.c_str(), "r");
        fseek(f,0,SEEK_END);
        int datalen=ftell(f);
        fseek(f,0,SEEK_SET);
        char *data=(char*)malloc(datalen);
        fread(data,1,datalen,f);
        fclose(f);
        
        SoundCloudCAPI_Parameter params[2];
        memset(params, 0, sizeof(SoundCloudCAPI_Parameter)*2);
        params[0].key="track[title]";
        params[0].value=namestr.c_str();
        params[0].filename=NULL;
        params[1].key="track[asset_data]";
        params[1].value=data;
        params[1].value_len=datalen;
        params[1].filename=fileForGenAndIndividual(gen,i).c_str();
        int err; void * rcvdata; unsigned long long size;
        SoundCloudCAPI_performMethod(scApi_,"POST","/me/tracks",params,2, &err, &rcvdata, &size);
        free(data);

        using boost::property_tree::ptree;
        ptree pt;
        istringstream is;
        is.str(std::string((char*)rcvdata, size));
        read_json(is,pt);
        // keep ID handy
        ids.push_back(pt.get<std::string>(ptree::path_type("id")));
        free(rcvdata);
    }
    return ids;
}

void SoundCloudServer::authenticate() const {
    if (authenticator_ == NULL) {
        authenticator_ = defaultAuthenticatorGenerator_(scApi_);
    }
    authenticated_ = authenticator_->authenticate();
}