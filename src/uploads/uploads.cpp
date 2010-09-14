/*
 *  uploads.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 4/18/10.
 *  Copyright 2010 Matt Kane. All rights reserved.
 *
 */

#include "uploads.h"

using namespace nynex;

void GrabUploadsAction::action(const GAGeneticAlgorithm & ga) {
    FILE *script = popen((std::string("/opt/nynex/bin/getuploads.pl ") + user_ + " " + server_ + " " + path_).c_str(), "r");
    char buf[FILENAME_MAX];
    while (fgets(buf, FILENAME_MAX, script) != NULL) {
        buf[strlen(buf) - 2] = '\0'; // get rid of newlines
        SampleBank::getInstance().addSample(buf);
    }
    pclose(script);
}