#ifndef NYNEX_STREAMING
#define NYNEX_STREAMING

namespace nynex {
    class Streamer {
    public:
        std::string submitSounds(const std::vector<std::string> & soundcloudUrls) const;
        // updates stream, creates listen page, returns listen page url
    private:
        std::string streamHost_;
        std::string webHost_;
    };
}

#endif