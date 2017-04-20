#include <string>
#include <vector>

#include <curl/curl.h>

#include "rtl_exec.h"
#include "utf8string.h"

static size_t header_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::vector<utf8string> *headers = reinterpret_cast<std::vector<utf8string> *>(userdata);
    std::string s(ptr, size*nmemb);
    auto i = s.find_last_not_of("\r\n");
    headers->push_back(s.substr(0, i+1));
    return size*nmemb;
}

static size_t data_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::string *data = reinterpret_cast<std::string *>(userdata);
    data->append(std::string(ptr, size*nmemb));
    return size*nmemb;
}

namespace rtl {

namespace http {

std::string get(const std::string &url, const std::map<utf8string, utf8string> &requestHeaders, std::vector<utf8string> *responseHeaders)
{
    std::string data;
    responseHeaders->clear();
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Neon/0.1");
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, responseHeaders);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    struct curl_slist *headerlist = NULL;
    std::vector<std::string> headervector;
    for (auto h: requestHeaders) {
        headervector.push_back(h.first.str() + ": " + h.second.str());
        headerlist = curl_slist_append(headerlist, headervector.back().c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    char error[CURL_ERROR_SIZE];
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
    CURLcode r = curl_easy_perform(curl);
    curl_slist_free_all(headerlist);
    if (r == CURLE_OK) {
        long rc;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rc);
        //printf("rc %ld\n", rc);
    } else {
        curl_easy_cleanup(curl);
        //fprintf(stderr, "curl %d error %s\n", r, error);
        throw RtlException(Exception_HttpException, error, r);
    }
    curl_easy_cleanup(curl);
    return data;
}

std::string post(const std::string &url, const std::string &post_data, const std::map<utf8string, utf8string> &requestHeaders, std::vector<utf8string> *responseHeaders)
{
    std::string data;
    responseHeaders->clear();
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "Neon/0.1");
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, header_callback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, responseHeaders);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, data_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, post_data.length());
    struct curl_slist *headerlist = NULL;
    std::vector<std::string> headervector;
    for (auto h: requestHeaders) {
        headervector.push_back(h.first.str() + ": " + h.second.str());
        headerlist = curl_slist_append(headerlist, headervector.back().c_str());
    }
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
    char error[CURL_ERROR_SIZE];
    curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, error);
    CURLcode r = curl_easy_perform(curl);
    curl_slist_free_all(headerlist);
    if (r == CURLE_OK) {
        long rc;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rc);
        //printf("rc %ld\n", rc);
    } else {
        curl_easy_cleanup(curl);
        //fprintf(stderr, "curl %d error %s\n", r, error);
        throw RtlException(Exception_HttpException, error, r);
    }
    curl_easy_cleanup(curl);
    return data;
}

} // namespace http

} // namespace rtl
