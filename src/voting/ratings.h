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

namespace nynex {
    class Ratings {
    public:
        static Ratings* getInstance();
        void addRating(int id, int score);
    private:
        void getServerRatings();
        static Ratings * instance_;
        Ratings();
        std::map<int, std::vector<int> > scores_;
        std::string host_;
        int port_;
        std::string path_;
    };
    
}

#endif