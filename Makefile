CC := clang

CFLAGS := -g -Wall -Werror -Wno-unused-function -Wno-unused-variable


all: final


clean:

	rm -f final


final: final.c 
	$(CC) $(CFLAGS) -o final final.c -lpthread

