all: lab2 lab2-test

INC_DIR = ../include

# CFLAG = -g -O3 -Wall -Werror -Wno-error=unused-variable -std=gnu99 -I$(INC_DIR)
CFLAG = -g -O3 -Wall -Wno-unused-result -Werror -std=gnu99 -I$(INC_DIR)

CC = gcc

CFLAG-TEST = -static -g -O3 -Wall -Werror -Wno-error=unused-variable -std=gnu99 -I$(INC_DIR)
CC-TEST = arm-linux-gnueabihf-gcc

DEPS =  $(INC_DIR)/pins.h $(INC_DIR)/bbsignal.c $(INC_DIR)/gpio.c
DEPS-TEST =  $(INC_DIR)/pins.h $(INC_DIR)/bbsignal.c $(INC_DIR)/gpio-stub.c

lab2: lab2.c $(DEPS)
	$(CC) $(CFLAG) -o $@ $^

lab2-test: lab2-test.c $(DEPS-TEST)
	$(CC-TEST) $(CFLAG-TEST) -o $@ $^

clean:
	rm -rf lab2 lab2-test