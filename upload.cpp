#include "unix.h"
#include "version.h"

#include <curl/curl.h>
#include <memory>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

struct memory {
    char *response;
    size_t size;
};

static size_t write_data(void *data, size_t size, size_t nmemb, void *userp)
{
    size_t realsize = size * nmemb;
    struct memory *mem = (struct memory *)userp;

    char *ptr = (char *)realloc(mem->response, mem->size + realsize + 1);
    if (ptr == nullptr)
        return 0;  /* out of memory! */

    mem->response = ptr;
    memcpy(&(mem->response[mem->size]), data, realsize);
    mem->size += realsize;
    mem->response[mem->size] = 0;

    return realsize;
}

long UploadFile(const FXString &filename, const FXString &username, const FXString &password, FXString &outBody)
{
    long code = 0;
    CURL *ch;
    CURLcode success;
    ch = curl_easy_init();
    if (ch) {
        curl_mime *form;
        form = curl_mime_init(ch);
        if (form) {
            memory response = { 0 };
            curl_mimepart *field;
            field = curl_mime_addpart(form);
            curl_mime_name(field, "input");
            curl_mime_filedata(field, filename.text());
            field = curl_mime_addpart(form);
            curl_mime_name(field, "submit");
            curl_mime_data(field, "submit", CURL_ZERO_TERMINATED);

            curl_easy_setopt(ch, CURLOPT_URL, "https://www.eressea.kn-bremen.de/eressea/orders-php/upload.php");
            curl_easy_setopt(ch, CURLOPT_WRITEDATA, &response);
            curl_easy_setopt(ch, CURLOPT_WRITEFUNCTION, write_data);
            curl_easy_setopt(ch, CURLOPT_USERAGENT, CSMAP_APP_TITLE_VERSION);
            curl_easy_setopt(ch, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
            curl_easy_setopt(ch, CURLOPT_USERNAME, username.text());
            curl_easy_setopt(ch, CURLOPT_PASSWORD, password.text());
            curl_easy_setopt(ch, CURLOPT_MIMEPOST, form);
            success = curl_easy_perform(ch);
            if (success == CURLE_OK) {
                curl_easy_getinfo(ch, CURLINFO_RESPONSE_CODE, &code);
                free(response.response);
            }
            curl_mime_free(form);
        }
        curl_easy_cleanup(ch);
    }
    return code;
}
