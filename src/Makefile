all: server

clean:
	@rm -rf *.o
	@rm -rf server

server: main.o httpd.o
	gcc -static -O3 -o server $^

main.o: main.c httpd.h
	gcc -static -O3 -c -o main.o main.c

httpd.o: httpd.c httpd.h
	gcc -static -O3 -c -o httpd.o httpd.c

