#pragma once

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include <curl/curl.h>

#ifdef CURL_WRITEFUNC_ERROR
    #define WRITEFUNC_ERROR CURL_WRITEFUNC_ERROR
#else
    #define WRITEFUNC_ERROR 0
#endif

#define N_URL_MAXCHAR 1024

/***********************************************
*                  global state                *
***********************************************/

__attribute__((constructor)) void global_setup(void);
__attribute__((destructor)) void global_cleanup(void);

/***********************************************
*                 return codes                 *
***********************************************/

typedef enum {
    RQC_FOUND,
    RQC_NOT_FOUND,
    RQC_UNKNOWN_RESPONSE,
    RQC_TIMEOUT,
    RQC_ERROR,
    RQC_BAD_ID,
} RequestsCode;

/***********************************************
*                buffer handling               *
***********************************************/

struct Buffer {
    uint32_t size;
    char *buffer;
    uint32_t maxSize;
};

#define PBUFFER_ADD_CHAR(pbuf, c) \
    pbuf->buffer[pbuf->size++] = c;

// add a zero-terminated string to buffer
#define PBUFFER_ADD_ZTSTR(pbuf, str) \
    strcpy(pbuf->buffer + pbuf->size, str); \
    pbuf->size += strlen(str); \

// add N bytes from str to buffer
#define PBUFFER_ADD_STRN(pbuf, str, N) \
    strncpy(pbuf->buffer + pbuf->size, str, N); \
    pbuf->size += N;

#define PBUFFER_REMAINING_CAPACITY(pbuf)\
    (pbuf->maxSize - pbuf->size)

/***********************************************
*                 data getters                 *
***********************************************/

// BibTex entries are usually around 500-700 characters
// Please allow for at least 1k bytes in bibData
// arxivID requires JUST the numbers WITHOUT the arvix:
// > i.e. 1003.2133
RequestsCode getBibTexarXiv(const char* arxivId, const uint32_t Nchar, char* bibData, uint32_t *Ndata, const uint32_t NdataMax, const long connTimeout, const long totTimeout, long *responseCode);

// BibTex entries are usually around 500-700 characters
// Please allow for at least 1k bytes in bibData
// DOI requires JUST the doi WITHOUT the doi.org/
// > i.e. 10.1140/epjh/e2010-00008-5
// > NOT https://doi.org/10.1140/epjh/e2010-00008-5
RequestsCode getBibTexDOI(const char* DOI, const uint32_t Nchar, char* bibData, uint32_t *Ndata, const uint32_t NdataMax, const long connTimeout, const long totTimeout, long *responseCode);

#ifdef __cplusplus
}
#endif
