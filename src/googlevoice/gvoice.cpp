/*
 *  gvoice.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 4/24/10.
 *  Copyright 2010 Matt Kane. All rights reserved.
 *
 */

#include "gvoice.h"
void nynex::GVoiceAction::action(const GAGeneticAlgorithm & ga) {
    FILE *script = popen((std::string("python /opt/nynex/bin/gv.py")).c_str(), "r");
    char buf[FILENAME_MAX];
    while (fgets(buf, FILENAME_MAX, script) != NULL) {
        buf[strlen(buf) - 1] = '\0'; // get rid of newline
        SampleBank::getInstance().addSample(buf);
    }
    pclose(script);
}
