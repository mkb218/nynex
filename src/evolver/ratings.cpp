/*
 *  ratings.cpp
 *  nynex
 *
 *  Created by Matthew J Kane on 1/27/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#include "ratings.h"
#include <fstream>

using namespace nynex;

Ratings * Ratings::instance_ = NULL;

Ratings & Ratings::getInstance() {
    if (instance_ == NULL) {
        instance_ = new Ratings();
    }
    return *instance_;
}

void Ratings::addRating(unsigned int id, int score) {
    scores_[id].push_back(score);
}

double Ratings::avgRatingForId(unsigned int id) {
    std::list<int> & list = scores_[id];
    double sum = 0.;
    unsigned int count = 0;
    for (std::list<int>::iterator it = list.begin(); it != list.end(); ++it) {
        sum += *it;
        ++count;
    }
    return sum/count;
}

void Ratings::getServerRatings() {
    std::string cmd(Ratings::getInstance().getScpCmd());
    const std::string & tmp(Ratings::getInstance().getTmpPath());
    cmd.append(" ");
    cmd.append(tmp);
    system(cmd.c_str()); // TODO check return val
    std::ifstream ifs(tmp.c_str());
    std::string line;
    while (!ifs.eof() && !ifs.fail()) {
        ifs >> line;
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            Ratings::getInstance().addRating(fromString<unsigned int>(line.substr(0, pos-1)),fromString<int>(line.substr(pos-1)));
        }
    }
    unlink(tmp.c_str());
}