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

#include "composition.h"

namespace nynex {
    class Ratings {
    public:
        static Ratings & getInstance();
        void addRating(unsigned int id, int score);
        double avgRatingForId(unsigned int id);
        void getServerRatings() {}
        void setHost(const std::string & host) { host_ = host; }
        void setPort(int port) { port_ = port; }
        void setPath(const std::string & path) { path_ = path; }
        void deleteRatingsForId(unsigned int id) { scores_.erase(id); }
        void deleteRatings() { scores_.clear(); }
    private:
        static Ratings * instance_;
        Ratings() {}
        Ratings(Ratings &) {}
        Ratings & operator=(Ratings&) {return *this;}
        std::map<unsigned int, std::list<int> > scores_;
        std::string host_;
        int port_;
        std::string path_;
    };
    
}

#endif