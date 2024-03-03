#include "httpd.h"
#include <sys/stat.h>

#define CHUNK_SIZE 1024 // read 1024 bytes at a time

int main(int c, char **v) {
  char *port = c == 1 ? "8000" : v[1];
  serve_forever(port);
  return 0;
}

void route() {
  ROUTE_START()

  GET(uri) {

    HTTP_302;
    printf("Content-Length: 0\n");
    printf("Location: https://github.com%s\n\n", uri);
  }

  ROUTE_END()
}
