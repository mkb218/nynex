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
#include <boost/foreach.hpp>
#undef check
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

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
    int ret = system(cmd.c_str()); // TODO check return val
    if (ret != 0) {
        return;
    }
    std::ifstream ifs((tmp+"/ratings.json").c_str());

    using boost::property_tree::ptree;
    ptree pt;
    read_json(ifs,pt);

    BOOST_FOREACH(ptree::value_type & v, pt) {
        Ratings::getInstance().addRating(fromString<unsigned int>(v.first),fromString<int>(v.second.data()));
    }
    unlink(tmp.c_str());
}