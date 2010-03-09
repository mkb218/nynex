/*
 *  soundcloud.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 1/28/10.
 *  Copyright 2010 Matthew J Kane. All rights reserved.
 *
 */

#include "soundcloud.h"
#include "SoundCloudCAPI.h" // this header has extern "C" 
#include <iostream>

using namespace nynex;

SoundCloudAuthenticator * SoundCloudServer::defaultAuthenticator = new TerminalAuthenticator();

bool TerminalAuthenticator::authenticate() {
    while (!authserver()) {
        std::string buffer;
        std::cout << "Please Auth, then enter the verification code here: " << std::flush;
        SoundCloudCAPI_SetVerifier(scAPI, buffer.c_str());
    }
}