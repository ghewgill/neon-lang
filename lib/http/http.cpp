#include <string>
#include <vector>

#include <curl/curl.h>

#include "neonext.h"

const char *Exception_HttpException = "HttpException";

const Ne_MethodTable *Ne;

static size_t header_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    Ne_Cell *headers = reinterpret_cast<Ne_Cell *>(userdata);
    std::string s(ptr, size*nmemb);
    auto i = s.find_last_not_of("\r\n");
    Ne->cell_set_string(Ne->cell_set_array_cell(headers, Ne->cell_get_array_size(headers)), s.substr(0, i+1).c_str());
    return size*nmemb;
}

static size_t data_callback(char *ptr, size_t size, size_t nmemb, void *userdata)
{
    std::string *data = reinterpret_cast<std::string *>(userdata);
    data->append(std::string(ptr, size*nmemb));
    return size*nmemb;
}

extern "C" {

Ne_EXPORT int Ne_INIT(const Ne_MethodTable *methodtable)
{
    Ne = methodtable;
    return Ne_SUCCESS;
}

Ne_FUNC(Ne_get)
{
    std::string url = Ne_PARAM_STRING(0);
    const Ne_Cell *requestHeaders = Ne_IN_PARAM(1);
    Ne_Cell *responseHeaders = Ne_OUT_PARAM(0);

    std::string data;
    Ne->cell_array_clear(responseHeaders);
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
    int n = Ne->cell_get_dictionary_size(requestHeaders);
    for (int i = 0; i < n; i++) {
        std::string h = Ne->cell_get_dictionary_key(requestHeaders, i);
        std::string v = Ne->cell_get_string(Ne->cell_get_dictionary_cell(requestHeaders, h.c_str()));
        headervector.push_back(h + ": " + v);
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
        Ne_RAISE_EXCEPTION(Exception_HttpException, error, r);
    }
    curl_easy_cleanup(curl);
    Ne_RETURN_BYTES(reinterpret_cast<const unsigned char *>(data.data()), static_cast<int>(data.size()));
}

Ne_FUNC(Ne_post)
{
    std::string url = Ne_PARAM_STRING(0);
    std::string post_data = Ne_PARAM_STRING(1);
    const Ne_Cell *requestHeaders = Ne_IN_PARAM(2);
    Ne_Cell *responseHeaders = Ne_OUT_PARAM(0);

    std::string data;
    Ne->cell_array_clear(responseHeaders);
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
    int n = Ne->cell_get_dictionary_size(requestHeaders);
    for (int i = 0; i < n; i++) {
        std::string h = Ne->cell_get_dictionary_key(requestHeaders, i);
        std::string v = Ne->cell_get_string(Ne->cell_get_dictionary_cell(requestHeaders, h.c_str()));
        headervector.push_back(h + ": " + v);
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
        Ne_RAISE_EXCEPTION(Exception_HttpException, error, r);
    }
    curl_easy_cleanup(curl);
    Ne_RETURN_BYTES(reinterpret_cast<const unsigned char *>(data.data()), static_cast<int>(data.size()));
}

} // extern "C"
