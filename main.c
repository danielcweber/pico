#include "httpd.h"
#include <sys/stat.h>

#define CHUNK_SIZE 1024 // read 1024 bytes at a time

int main(int c, char** v)
{
	char* port = "8000";
	char* redirect_target = "https://github.com";

	for (int i = 1; i < c - 1; i++)
	{
		if (strcmp("-p", v[i]) == 0)
		{
			port = v[i + 1];
			i++;
		}
		else if (strcmp("-t", v[i]) == 0)
		{
			redirect_target = v[i + 1];
			i++;
		}
	}

	serve_forever(port, redirect_target);

	return 0;
}