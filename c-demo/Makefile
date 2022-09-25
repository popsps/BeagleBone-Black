all: hw2 hw2-test
file:///home/must/Desktop/BeagleBone-Black/.gitignore

CFLAG-TEST=-g -O3 -Wall -Werror -Wno-error=unused-variable -std=gnu99
CC-TEST=gcc
CFLAG-TEST=-static -g -O3 -Wall -Werror -Wno-error=unused-variable -std=gnu99
CC-TEST=arm-linux-gnueabihf-gcc

hw2: hw2.c pins.h signal.c gpio-stub.c
	$(CC) $(CFLAG) -o $@ $^

hw2-test: hw2-test.c pins.h signal.c gpio-stub.c
	$(CC-TEST) $(CFLAG-TEST) -o $@ $^

clean:
	rm -rf hw2 hw2-test