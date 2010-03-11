#include "twitter.h"

#include <sstream>
#include <iomanip>

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

void TwitterServer::announceGeneration(const std::string & announcement, const std::string & listenUrl, const std::string & htmlUrl) {
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
}