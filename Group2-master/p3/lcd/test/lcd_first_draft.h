/* HEADER FOR LCD_FIRST_DRAFT.C FILE */

#include<linux/delay.h>
#include<linux/gpio.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/cdev.h>
#include <linux/semaphore.h>
#include <asm/uaccess.h>

#define DEVICE_NAME "lcd_first_draft"

/* function declarations */
static int __init driver_entry(void);
static void __exit driver_exit(void);
static int  device_open(struct inode*, struct file*);
static int device_close(struct inode*, struct file *);
static ssize_t device_read(struct file*, char*, size_t, loff_t*);
static ssize_t device_write(struct file*, const char*, size_t, loff_t*);
// functions that will probably be removed later
int checkGpio(int gpioNum);

/* data structures */
// contains data about the device.
// data : buffer for character data stored.
// sem  : semaphore.
struct fake_device {
	char data[100];
	struct semaphore sem;
} virtual_device;


/* global variables */
// stores info about this char device.
static struct cdev* mcdev;
// holds major and minor number granted by the kernel
static dev_t dev_num;


/* operations usable by this file. */
static struct file_operations fops = {
   .owner = THIS_MODULE,
   .read = device_read,
   .write = device_write,
   .open = device_open,
   .release = device_close
};


