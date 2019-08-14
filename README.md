# blinkt_kmod
blinkt_kmod can control LED stick "Blinkt!" in Kernel module.
# What is "Blinkt!"
Blinkt! is a strip of LED lights that you can control with your Raspberry Pi.
# How to use
## 1. Build kernel module for Raspberry Pi

## 2. Create device file
- Create a device file to light each LED.
- Set the major number to 240, and the minor number specifies the LED number you want to light.
- Minor number 0 controls all LEDs.
```
$ mkdev led0 c 240 0 # Minor number 0 controls all LEDs.
$ mkdev led1 c 240 1 # Minor number 1 controls 1st LED.

...
$ mkdev led1 c 240 8 #


```
## 3. Write values to the created device file
```
$ echo -n $'1' > led1 # No.1 LED light!
$ echo -n $'\xff' > led0 # All LEDs light!

```
# LICENSE
GPL v2

See [LICENSE](/LICENSE).
