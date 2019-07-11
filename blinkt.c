#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/module.h>
#include <linux/gpio.h>
#include <linux/delay.h>
void send_bit(int value)
{
	gpio_set_value(23,value);
	gpio_set_value(24,1);
	ndelay(500);
	gpio_set_value(24,0);
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
	gpio_request(23,"sysfs");
	gpio_request(24,"sysfs");
	gpio_direction_output(23,0);
	gpio_direction_output(24,0);

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
	gpio_free(23);
	gpio_free(24);
	
}
static int __init blinkt_init(void)
{
	if(!gpio_is_valid(23)){
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
