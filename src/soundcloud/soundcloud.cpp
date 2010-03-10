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

void SoundCloudServer::fetchDropBox() const {
    void *data;
    unsigned long long datalen;
    int errnum;
    SoundCloudCAPI_Parameter filter = { "filter", "drop", 0, NULL };
    int res = SoundCloudCAPI_performMethod(scApi_, "GET", "/events", &filter, 1 ,&errnum, &data, &datalen);
    if (!res) {
        std::cout << "something wrong" << std::endl;
    }
    
}