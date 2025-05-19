#include "requests.h"

__attribute__((constructor)) void global_setup(void){
    curl_global_init(CURL_GLOBAL_DEFAULT);
}

__attribute__((destructor)) void global_cleanup(void){
    curl_global_cleanup();
}

static size_t write_callback(char *ptr, size_t size, size_t nmemb, void *userdata){
    (void) size;
    if(nmemb==0){
        // zero-byte -> failed request
        return 0;
    }
    struct Buffer *pbuf=(struct Buffer*)userdata;
    if(PBUFFER_REMAINING_CAPACITY(pbuf)<nmemb){
        return WRITEFUNC_ERROR; // we have ran out of buffer...
    }
    PBUFFER_ADD_STRN(pbuf, ptr, nmemb);
    return nmemb;
}

RequestsCode getBibTexarXiv(const char* arxivId, const uint32_t Nchar, char* bibData, uint32_t *Ndata, const uint32_t NdataMax, const long connTimeout, const long totTimeout, long *responseCode){
    (void) Nchar;

    struct Buffer buf;
    buf.buffer = bibData;
    buf.maxSize = NdataMax;
    buf.size = 0;
    struct Buffer *pbuf= &buf;

    CURL *curl;
    CURLcode res;
    CURLUcode Ures;

    RequestsCode returnCode;

    curl = curl_easy_init();
    if(!curl){
        return RQC_ERROR;
    }

    CURLU *urlp = curl_url();

    char url[N_URL_MAXCHAR]; // This should be plenty
    snprintf(url, N_URL_MAXCHAR, "https://arxiv.org/bibtex/%s", arxivId); //automatically zero-terminated
    
    Ures = curl_url_set(urlp, CURLUPART_URL, url, CURLU_DISALLOW_USER);
    // let us avoid people putting funny stuff in their bibs...
    if(Ures!= CURLUE_OK){
        return RQC_ERROR;
    }
    curl_easy_setopt(curl, CURLOPT_CURLU, urlp);

    // The arxiv is probs OK, but, ya know....
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // cache CA for a day
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 86400L);

    // setup timeouts
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connTimeout);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, totTimeout);

    // set up the output
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, pbuf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    // perform the thing
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        if(res == CURLE_OPERATION_TIMEDOUT){
            return RQC_TIMEOUT;
        }
        return RQC_ERROR;
    }

    PBUFFER_ADD_CHAR(pbuf,' ');
    PBUFFER_ADD_CHAR(pbuf,0); // make sure the string it's zero-terminated

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode);
    if(*responseCode == 200L){
        returnCode = RQC_FOUND;
    } else if (*responseCode == 404L){
        returnCode = RQC_NOT_FOUND;
    } else {
        returnCode = RQC_UNKNOWN_RESPONSE;
    }

    *Ndata = pbuf->size;

    curl_easy_cleanup(curl);
    return returnCode;
}

RequestsCode getBibTexDOI(const char* DOI, const uint32_t Nchar, char* bibData, uint32_t *Ndata, const uint32_t NdataMax, const long connTimeout, const long totTimeout, long *responseCode){
    (void) Nchar;

    struct Buffer buf;
    buf.buffer = bibData;
    buf.maxSize = NdataMax;
    buf.size = 0;
    struct Buffer *pbuf= &buf;

    CURL *curl;
    CURLcode res;
    CURLUcode Ures;
    struct curl_slist *headerList = NULL;

    RequestsCode returnCode;

    curl = curl_easy_init();
    if(!curl){
        return RQC_ERROR;
    }

    CURLU *urlp = curl_url();

    char url[N_URL_MAXCHAR]; // This should be plenty
    snprintf(url, N_URL_MAXCHAR, "https://doi.org/%s", DOI); //automatically zero-terminated
    
    Ures = curl_url_set(urlp, CURLUPART_URL, url, CURLU_DISALLOW_USER);
    // let us avoid people putting funny stuff in their bibs...
    if(Ures!= CURLUE_OK){
        return RQC_ERROR;
    }
    curl_easy_setopt(curl, CURLOPT_CURLU, urlp);

    // The arxiv is probs OK, but, ya know....
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L);

    // cache CA for a day
    curl_easy_setopt(curl, CURLOPT_CA_CACHE_TIMEOUT, 86400L);

    // setup timeouts
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connTimeout);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, totTimeout);

    // set up the output
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, pbuf);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);

    // set the right headers
    headerList = curl_slist_append(headerList, "Accept: text/bibliography; style=bibtex; locale=en-GB");
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerList);

    // make sure we enable redirects
    // we will be bounced through a couple of endpoints
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

    // perform the thing
    res = curl_easy_perform(curl);
    if(res != CURLE_OK){
        if(res == CURLE_OPERATION_TIMEDOUT){
            return RQC_TIMEOUT;
        }
        return RQC_ERROR;
    }

    PBUFFER_ADD_CHAR(pbuf,' ');
    PBUFFER_ADD_CHAR(pbuf,0); // make sure the string it's zero-terminated

    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, responseCode);
    if(*responseCode == 200L){
        returnCode = RQC_FOUND;
    } else if (*responseCode == 404L){
        returnCode = RQC_NOT_FOUND;
    } else {
        returnCode = RQC_UNKNOWN_RESPONSE;
    }

    *Ndata = pbuf->size;

    curl_easy_cleanup(curl);
    curl_slist_free_all(headerList);
    return returnCode;
}
