# HW2

Running the application

```shell
# run this
gcc -O2 -Wall pins.h signal.c gpio.c hw2.c -o hw2
# or
make hw2
./hw2
```

Running the stub for testing

```shell
# run this
arm-linux-gnueabihf-gcc -static -O2 -Wall pins.h signal.c  gpio-stub.c hw2-test.c -o hw2-test
# or
make hw2-test
./hw2-test
```
