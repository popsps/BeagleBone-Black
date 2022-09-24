# HW2

Running the application

```shell
gcc -O2 -Wall pins.h signal.c gpio.c hw2.c -o hw2
```

Running the stub for testing

```shell
arm-linux-gnueabihf-gcc -static -O2 -Wall pins.h signal.c  gpio-stub.c hw2-test.c -o hw2-test
```
