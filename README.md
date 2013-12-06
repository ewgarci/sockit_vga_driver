sockit_vga_driver
=================

Driver for Lab 3 of Embedded Systems Class


Command to make dtb from dts file 

dtc -O dtb -I dts -o lab_3.dtb  lab_3.dts

Command to make Kernel Module. 

make ARCH=arm CROSS_COMPILE=~/gcc-linaro-arm-linux-gnueabihf-4.7-2012.11-20121123_linux/bin/arm-linux-gnueabihf-

Cross Compiled main.c with  

~/gcc-linaro-arm-linux-gnueabihf-4.7-2012.11-20121123_linux/bin/arm-linux-gnueabihf-gcc

Build with the Linux 3.7 Kernel sources from Altera


