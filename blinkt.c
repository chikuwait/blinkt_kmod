#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>

#define DATA_PIN 23
#define CLOCK_PIN 24
struct cdev *cdev;
dev_t dev;
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
	int i;
	for (i = 0; i < 8; i++) {
		send_bit((value >> (7 - i)) & 0x01);
	}
}
void start_control(void)
{
	int i;
	gpio_request(DATA_PIN, "sysfs");
	gpio_request(CLOCK_PIN, "sysfs");
	gpio_direction_output(DATA_PIN, 0);
	gpio_direction_output(CLOCK_PIN, 0);

	for (i = 0; i < 32; i++) {
		send_bit(0);
	}
}
void end_control(void)
{
	int n;
	for (n = 0; n < 32; n++) {
		send_bit(1);
	}
	gpio_free(DATA_PIN);
	gpio_free(CLOCK_PIN);
}

static ssize_t chardev_write(struct file *p, const char __user *usr,
			     size_t size, loff_t *loff)
{
	char value;
	int i, n;

	if (copy_from_user(&value, usr, size) != 0) {
		return -EFAULT;
	}
	if (!gpio_is_valid(DATA_PIN)) {
		return -ENODEV;
	}

	start_control();
	for (n = 0; n < 8; n++) {
		if (((value >> (7 - n)) & 0x01) == 1) {
			send_byte(20 | 0xe0);
			send_byte(255);
		} else {
			send_byte(0 | 0xe0);
			send_byte(0);
		}
		send_byte(0);
		send_byte(0);
	}
	end_control();

	return size;
}
static struct file_operations chardev_fops = {
    .owner = THIS_MODULE,
    .write = chardev_write,
};

static int __init blinkt_init(void)
{
	int err;
	dev = MKDEV(240, 0);
	register_chrdev_region(dev, 1, "hello");
	cdev = cdev_alloc();
	cdev->ops = &chardev_fops;
	err = cdev_add(cdev, dev, 1);

	return 0;
}

static void __exit blinkt_exit(void)
{
	int i;
	cdev_del(cdev);
	unregister_chrdev_region(dev, 1);
	start_control();
	for (i = 0; i < 8; i++) {
		send_byte(0 | 0xe0);
		send_byte(0);
		send_byte(0);
		send_byte(0);
	}
	end_control();
	return;
}

module_init(blinkt_init);
module_exit(blinkt_exit);

MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("chikuwait");
