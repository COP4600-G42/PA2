obj-m += pa2.o

all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) modules
	$(CC) testpa2.c -o testpa2

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(shell pwd) clean
	rm testpa2
