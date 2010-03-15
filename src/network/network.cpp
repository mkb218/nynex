#include "network.h"
#include <libkern/OSAtomic.h>
#include "composition.h"

using namespace nynex;

volatile CurlContext *CurlContext::context_ = NULL;
volatile bool CurlContext::init_ = false;

volatile CurlContext * CurlContext::getContext() {
    if (!OSAtomicTestAndSet(7, &CurlContext::init_)) {
        context_ = new CurlContext();
    }
    return context_; 
}

void getUrl(const std::string & url, const std::string & user = "", const std::string & password = "") {
    char errorbuf[CURL_ERROR_SIZE];
    volatile CurlContext *context = CurlContext::getContext();
    CURL * curl = curl_easy_init();
    if (curl == NULL) throw std::runtime_error("curl easy init failed!");
    try {
        CURLcode result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (result != CURLE_OK) {
            throw std::runtime_error("setopt URL failed: " + stringFrom(result));
        }
        
        if (user.size() > 0 && password.size() > 0) {
            result = curl_easy_setopt(curl, CURLOPT_USERNAME, user.c_str());
            if (result != CURLE_OK) {
                throw std::runtime_error("setopt USER failed: " + stringFrom(result));
            }

            result = curl_easy_setopt(curl, CURLOPT_PASSWORD, user.c_str());
            if (result != CURLE_OK) {
                throw std::runtime_error("setopt PASS failed: " + stringFrom(result));
            }
        }
        
        result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            throw std::runtime_error("setopt PERFORM failed: " + stringFrom(result) + " " + errorbuf);
        }
        
    } catch (...) {
        curl_easy_cleanup(curl);
        throw;
    }
    curl_easy_cleanup(curl);
}


void postUrl(const std::string & url, const std::string & body, const std::string & user = "", const std::string & password = "") {
    char errorbuf[CURL_ERROR_SIZE];
    volatile CurlContext *context = CurlContext::getContext();
    CURL * curl = curl_easy_init();
    if (curl == NULL) throw std::runtime_error("curl easy init failed!");
    try {
        CURLcode result = curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (result != CURLE_OK) {
            throw std::runtime_error("setopt URL failed: " + stringFrom(result));
        }

        result = curl_easy_setopt(curl, CURLOPT_POST, 1);
        if (result != CURLE_OK) {
            throw std::runtime_error("setopt POST failed: " + stringFrom(result));
        }
        
        result = curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.c_str());
        if (result != CURLE_OK) {
            throw std::runtime_error("setopt POSTFIELDS failed: " + stringFrom(result));
        }
        
        if (user.size() > 0 && password.size() > 0) {
            result = curl_easy_setopt(curl, CURLOPT_USERNAME, user.c_str());
            if (result != CURLE_OK) {
                throw std::runtime_error("setopt USER failed: " + stringFrom(result));
            }
            
            result = curl_easy_setopt(curl, CURLOPT_PASSWORD, user.c_str());
            if (result != CURLE_OK) {
                throw std::runtime_error("setopt PASS failed: " + stringFrom(result));
            }
        }
        
        result = curl_easy_perform(curl);
        if (result != CURLE_OK) {
            throw std::runtime_error("setopt PERFORM failed: " + stringFrom(result) + " " + errorbuf);
        }
        
    } catch (...) {
        curl_easy_cleanup(curl);
        throw;
    }
    curl_easy_cleanup(curl);
}