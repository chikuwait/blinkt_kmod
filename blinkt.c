#include <linux/cdev.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/gpio.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/uaccess.h>
#include <linux/mutex.h>

#define DATA_PIN 23
#define CLOCK_PIN 24
static DEFINE_MUTEX(my_mutex);
static DEFINE_MUTEX(readmutex);

struct cdev cdev[9];
dev_t dev;

char pinvalue = 0x00;

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

static ssize_t chardev_write(struct file *p, const char __user * usr,
			     size_t size, loff_t * loff)
{
	if (mutex_lock_interruptible(&my_mutex) != 0) {
		return -EFAULT;
	}
	char buf = 0x00;
	int n, minor;
	if (copy_from_user(&buf, usr, size) != 0) {
		return -EFAULT;
	}
	if (!gpio_is_valid(DATA_PIN)) {
		return -ENODEV;
	}

	minor = (int *)(p->private_data);
	if (minor != 0) {
		if (buf == 0x01) {
			buf = pinvalue | (buf << (minor - 1));
		} else {
			buf = pinvalue ^ (1 << (minor - 1));
		}
	}
	pinvalue = buf;
	printk("minor = %d\n", minor);
	start_control();
	for (n = 0; n < 8; n++) {
		if (((pinvalue >> (7 - n)) & 0x01) == 1) {
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

	mutex_unlock(&my_mutex);
	return size;
}

static ssize_t chardev_read(struct file *p, char __user * usr, size_t size,
			    loff_t * loff)
{
	if (mutex_lock_interruptible(&my_mutex) != 0) {
		return -EFAULT;
	}
	char buf = 0x00;
	int minor = (int *)(p->private_data);
	if (minor != 0) {
		buf = (pinvalue >> (minor - 1)) & 0x01;
	} else {
		buf = pinvalue;
	}
	if (copy_to_user(usr, &buf, size) != 0) {
		return -EFAULT;
	}
	printk("pivalue = %0x\n", pinvalue);
	mutex_unlock(&my_mutex);
	return 1;
}

static int chardev_open(struct inode *inode, struct file *filep)
{
	int minor = iminor(inode);
	filep->private_data = (void *)minor;
	return 0;
}

static struct file_operations chardev_fops = {
	.owner = THIS_MODULE,
	.write = chardev_write,
	.read = chardev_read,
	.open = chardev_open,
};

static int __init blinkt_init(void)
{
	int i;

	for (i = 0; i < 9; i++) {
		dev = MKDEV(240, i);
		register_chrdev_region(dev, i, "hello");
		cdev_init(&cdev[i], &chardev_fops);
		cdev[i].owner = THIS_MODULE;
		cdev_add(&cdev[i], dev, 1);
	}
	return 0;
}

static void __exit blinkt_exit(void)
{
	int i;
	for (i = 0; i < 9; i++) {
		cdev_del(&cdev[i]);
	}

	unregister_chrdev_region(dev, 9);
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
