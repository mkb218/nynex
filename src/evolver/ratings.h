/*
 *  ratings.h
 *  nynex
 *
 *  Created by Matthew J Kane on 1/27/10.
 *  Copyright 2010 Matthew Kane. All rights reserved.
 *
 */

#ifndef NYNEX_RATINGS
#define NYNEX_RATINGS

#include <map>
#include <string>
#include <list>

#include "evolver.h"
#include "composition.h"

namespace nynex {
    class Ratings {
    public:
        static Ratings & getInstance();
        void addRating(unsigned int id, int score);
        bool hasRatingForId(unsigned int id) { return (scores_.find(id) != scores_.end()); }
        double avgRatingForId(unsigned int id);
        void getServerRatings();
        void setScpCmd(const std::string & cmd) { cmd_ = cmd; }
        void setTmpPath(const std::string & path) { tmppath_ = path; }
        const std::string & getScpCmd() { return cmd_; }
        const std::string & getTmpPath() { return tmppath_; }
        void deleteRatingsForId(unsigned int id) { scores_.erase(id); }
        void deleteRatings() { scores_.clear(); }
    private:
        static Ratings * instance_;
        Ratings() {}
        Ratings(Ratings &) {}
        Ratings & operator=(Ratings&) {return *this;}
        std::map<unsigned int, std::list<int> > scores_;
        std::string cmd_;
        std::string tmppath_;
    };
    
}

#endif