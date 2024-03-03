#include "httpd.h"
#include <sys/stat.h>

#define CHUNK_SIZE 1024 // read 1024 bytes at a time

int main(int c, char **v) {
  char *port = c == 1 ? "8000" : v[1];
  char *redirect_target = c == 2 ? "https://github.com" : v[2];
  serve_forever(port, redirect_target);
  return 0;
}