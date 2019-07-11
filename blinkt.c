#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>

#define DATA_PIN 23
#define CLOCK_PIN 24
void send_bit(int value)
{
	gpio_set_value(DATA_PIN, value);
	gpio_set_value(CLOCK_PIN, 1);
	ndelay(500);
	gpio_set_value(CLOCK_PIN, 0);
	ndelay(500);

}
void send_byte(int value)
{
	int i=0;
	for(i=0;i<8;i++){
		send_bit((value>>(7-i)) & 0x01);
	}
}
void start_control(void)
{
	gpio_request(DATA_PIN, "sysfs");
	gpio_request(CLOCK_PIN, "sysfs");
	gpio_direction_output(DATA_PIN, 0);
	gpio_direction_output(CLOCK_PIN, 0);

	int i=0;
	for(i=0;i<32;i++){
		send_bit(0);
	}

}
void end_control(void)
{
	int n=0;
	for(n=0;n<32;n++){
		send_bit(1);
	}
	gpio_free(DATA_PIN);
	gpio_free(CLOCK_PIN);
	
}
static int __init blinkt_init(void)
{
	if(!gpio_is_valid(DATA_PIN)){
		return -ENODEV;
	}
	start_control();
	int n;
	for(n=0;n<8;n++){
		send_byte(31|0xe0);
		send_byte(255);
		send_byte(0);
		send_byte(0);
	}
	end_control();
	return 0;
}

static void __exit blinkt_exit(void)
{
	int i=0;
	start_control();
	for(i=0;i<8;i++){
		send_byte(0|0xe0);
		send_byte(0);
		send_byte(0);
		send_byte(0);
	}
	end_control();
	return ;
}

module_init(blinkt_init);
module_exit(blinkt_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("chikuwait");
