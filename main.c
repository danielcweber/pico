#include "httpd.h"
#include <sys/stat.h>

#define CHUNK_SIZE 1024 // read 1024 bytes at a time

int main(int c, char **v) {
  char *port = c == 1 ? "8000" : v[1];
  serve_forever(port);
  return 0;
}