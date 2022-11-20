all: main main-test

INC_DIR = ../include

CFLAG = -pthread -g -O3 -Wall -Wno-unused-result -Wno-unused-variable -Werror -std=gnu99 -I$(INC_DIR)
CC = gcc

CFLAG-TEST = -static -pthread -g -O3 -Wall -Wno-unused-result -Werror -std=gnu99 -I$(INC_DIR)
CC-TEST = arm-linux-gnueabihf-gcc

DEPS =  $(INC_DIR)/pins.h $(INC_DIR)/bbsignal.c $(INC_DIR)/gpio.c $(INC_DIR)/logging.c
DEPS-TEST =  $(INC_DIR)/pins.h $(INC_DIR)/bbsignal.c $(INC_DIR)/gpio-stub.c $(INC_DIR)/logging.c

# run acutal program that will be shiped into the embded device
main: main.c $(DEPS)
	$(CC) $(CFLAG) -o $@ $^

# run the stub version on armvf compiler. This will be used for testing and cross-compilations.
main-semi-test: main.c $(DEPS-TEST)
	$(CC) $(CFLAG) -o $@ $^

# run the stub version on armvf compiler. This will be used for testing and cross-compilations.
main-test: main.c $(DEPS-TEST)
	$(CC-TEST) $(CFLAG-TEST) -o $@ $^


clean:
	rm -rf main main-semi-test main-test