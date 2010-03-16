#include "twitter.h"

#include <sstream>
#include <iomanip>
#include <boost/property_tree/json_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include "network.h"

using namespace nynex;

std::string TwitterServer::urlEncode(const std::string & in) {
    std::string out;
    for (size_t i = 0; i < in.size(); ++i) {
        char c = in[i];
        // assumes ASCII, not EBCDIC. :P
        if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') || (c >= '0' && c <= '9') || c == '-' || c == '_' || c == '.' || c == '~') {
            out.append(c);
        } else {
            out.append("%");
            ostringstream os;
            os << std::hex << std::setw(2) << std::setfill('0') << (int) c;
            out.append(os.str());
        }
    }
    return out;
}

void TwitterServer::announceGeneration(const std::string & announcement, const std::string & listenUrl, const std::string & htmlUrl) const {
    std::string tweet(announcement);
    tweet.append(" listen and rate: ");
    tweet.append(getBitlyUrl(htmlUrl));
    tweet.append(" stream: ");
    tweet.append(getBitlyUrl(listenUrl));
    while (tweet.size() > 140) {
        // remove hash tags
        size_type start, end;
        start = tweet.find('#');
        if (start != std::npos) {
            end = tweet.find(' ', start);
            if (end != std::npos) {
                tweet.erase(start, end - start + 1);
            } else {
                tweet.erase(start, tweet.size());
            }
        } else {
            // maybe more to do here
            tweet.erase(140, tweet.size());
        }
    }
//    tweet = urlEncode(tweet);
//    tweet = std::string("status=") + urlEncode(tweet);
    tweet = std::string("status=") + tweet;
    // todo get from config
    std::string tweetUrl("http://api.twitter.com/l/statuses/update.json")
    using boost::property_tree::ptree;
/*    ptree pt;
    pt.put("status", tweet);
    ostringstream os;
    write_json(os, pt);
    std::string tweetJson(os.str());*/
    postUrl(tweetUrl, tweet, username_, password_);
}

std::string getBitlyUrl(const std::string url) const {
    // todo get from config
    std::string request("http://api.bit.ly/shorten?format=json&version=2.0.1&longUrl=");
    request.append(url);
    request.append("&login=");
    request.append(bitlylogin_);
    request.append("&apiKey=");
    request.append(bitlykey_);
    std::string response = getUrl(request);

    // fish new url out of json response
    using boost::property_tree::ptree;
    ptree pt;
    istringstream is;
    is.str(response);
    read_json(is, pt);
    return pt.get<std::string>("results.shortUrl");
}

void TwitterAnnounce::action(const GAGeneticAlgorithm & ga) {
    std::string generation = stringFrom(ga.generation());
    // todo get from config
    announcer->announceGeneration("Generation " + generation + " created! #megapolis", "http://nynex.hydrogenproject.com/stream.php?generation="+generation, "http://nynex.hydrogenproject.com/listen.php?generation="generation);
}