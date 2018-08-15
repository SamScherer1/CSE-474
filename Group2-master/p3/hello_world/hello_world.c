#include<linux/init.h>
#include<linux/module.h>
#include<linux/kernel.h>
MODULE_LICENSE("Dual BSD/GPL");

static int __init hello_start(void)
{
	printk(KERN_INFO "Hello, world module!!\n");
	return 0;
}

static void __exit hello_exit(void)
{
	printk(KERN_INFO "Goodbye world!\n");
}

module_init(hello_start);
module_exit(hello_exit);

