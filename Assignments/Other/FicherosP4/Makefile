MODULE_NAME=chardev_leds

obj-m := ${MODULE_NAME}.o

all:	
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:	
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

