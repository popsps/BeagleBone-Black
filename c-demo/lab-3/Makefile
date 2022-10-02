all: lab3 lab3-test

INC_DIR = ../include

# CFLAG = -g -O3 -Wall -Werror -Wno-error=unused-variable -std=gnu99 -I$(INC_DIR)
CFLAG = -g -O3 -Wall -Wno-unused-result -Werror -std=gnu99 -I$(INC_DIR)
CC = gcc

CFLAG-TEST = -static -g -O3 -Wall -Werror -Wno-error=unused-variable -std=gnu99 -I$(INC_DIR)
CC-TEST = arm-linux-gnueabihf-gcc

DEPS =  $(INC_DIR)/pins.h $(INC_DIR)/bbsignal.c $(INC_DIR)/gpio.c
DEPS-TEST =  $(INC_DIR)/pins.h $(INC_DIR)/bbsignal.c $(INC_DIR)/gpio-stub.c

lab3: lab3.c $(DEPS)
	$(CC) $(CFLAG) -o $@ $^

lab3t: lab3-tt.c $(DEPS)
	$(CC) $(CFLAG) -o $@ $^

lab3-test: lab3-test.c $(DEPS)
	$(CC) $(CFLAG-TEST) -o $@ $^

clean:
	rm -rf lab3 lab3-test