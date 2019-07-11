gpio-objs := blinkt.o
obj-m := gpio.o
all:
	 make ARCH=arm64 CROSS_COMPILE=aarch64-linux-gnu- -C ../linux-stable M=`pwd` modules
setip:
	ip a a 192.168.42.2/24 dev enp0s31f6
