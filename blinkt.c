#include <linux/init.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/uaccess.h>
#include <linux/module.h>

static int __init blinkt_init(void)
{
	return 0;
}

static void __exit blinkt_exit(void)
{
	printk(KERN_INFO "Bye World\n");
	return ;
}

module_init(blinkt_init);
module_exit(blinkt_exit);

MODULE_LICENSE("MIT");
MODULE_AUTHOR("chikuwait");
