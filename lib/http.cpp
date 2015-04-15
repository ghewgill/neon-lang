#include <string>

#include <curl/curl.h>

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::string *data = reinterpret_cast<std::string *>(userdata);
    data->append(std::string(ptr, size*nmemb));
    return size;
}

namespace rtl {

std::string http$get(const std::string &url)
{
    std::string data;
    CURL *curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &data);
    CURLcode r = curl_easy_perform(curl);
    if (r == CURLE_OK) {
        long rc;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &rc);
    } else {
    }
    curl_easy_cleanup(curl);
    return data;
}

}
