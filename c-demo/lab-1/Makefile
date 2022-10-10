all: lab1 lab1-test



CFLAG = -g -O3 -Wall -Wno-unused-result -Werror -std=gnu99
CC = gcc

CFLAG-TEST = -static -pthread -g -O3 -Wall -Wno-unused-result -Werror -std=gnu99
CC-TEST = arm-linux-gnueabihf-gcc


# run acutal program that will be shiped into the embded device
lab1: lab1.c
	$(CC) $(CFLAG) -o $@ $^

# run the stub version on armvf compiler. This will be used for testing and cross-compilations.
lab1-test: lab1.c
	$(CC-TEST) $(CFLAG-TEST) -o $@ $^


clean:
	rm -rf lab1 lab1-test