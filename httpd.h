#ifndef _HTTPD_H___
#define _HTTPD_H___

#include <stdio.h>
#include <string.h>

// Client request
extern char *method, // "GET" or "POST"
    *uri,            // "/index.html" things before '?'
    *prot;           // "HTTP/1.1"

// Server control functions
void serve_forever(const char *PORT);

#endif
