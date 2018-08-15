// lcd_first_draft.c 
// Requests availability of GPIO 45 and when written to will write bit to 1, wait, then 0

#include "lcd_first_draft.h"

/********************* FILE OPERATION FUNCTIONS ***************/

// runs on startup
// intializes module space and declares major number.
// assigns device structure data.
static int __init driver_entry(void) {
	// code for the kernel to initialize module
	// needs to make mutex, minor/major numbers
	// needs to create and initialize cdev structure

	// REGISTERING OUR DEVICE WITH THE SYSTEM
	// (1) ALLOCATE DYNAMICALLY TO ASSIGN OUR DEVICE
	int ret = alloc_chrdev_region(&dev_num, 0, 1, DEVICE_NAME);
	if (ret < 0) {
		printk(KERN_ALERT "new_char: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "new_char: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// (2) CREATE CDEV STRUCTURE, INITIALIZING CDEV
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// After creating cdev, add it to kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "new_char: unable to add cdev to kernerl\n");
		return ret;
	}
	// Initialize SEMAPHORE
	sema_init(&virtual_device.sem, 1);
	return 0;
}


static void __exit driver_exit(void) {
	// code for kernel to wrap up module?
	cdev_del(mcdev);
	unregister_chrdev_region(dev_num, 1);
	printk(KERN_ALERT "Lcd: successfully unloaded / exited\n");
}




/* DEVICE OPEN */
// Arguments: inode reference to file on disk, struct file represents an abstract
// Functionality: check GPIO 45 valid, request GPIO 45
int device_open(struct inode *inode, struct file* filp) {
	// check if lcd is in use
	checkGpio(45);
	if(gpio_request(45, "serInput") < 0) {
		printk(KERN_ALERT "GPIO number %d failed on request\n", 45);
	} else {
		printk(KERN_INFO "GPIO number %d successfully requested\n", 45);
	}
	// if not, should initialize
	if (down_interruptible(&virtual_device.sem) != 0) {
		printk(KERN_ALERT "new_char: could not lock device during open\n");
		return -1;
	}
	
	printk(KERN_INFO "new_char: device opened\n");
	return 0;
}

// Called upon close
// closes device and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
	up(&virtual_device.sem);
	printk(KERN_INFO "new_char, closing device\n");
	
	// remove GPIO 45's taken status
	gpio_free(45);
	return 0;
}

// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "new_char: Reading from device...\n");
	return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}

/* WRITE TO DEVICE */
// Called when user wants to send info to device
// sets GPIO 45 to output and high for 5 seconds, then back to low
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "new_char: writing to device...\n");

	// set pin 45 to 1, wait, set to 0
	int outputSet = gpio_direction_output(45, 1);	// sets output direction and value for 45
	if (outputSet < 0) {
		printk(KERN_ALERT "failed to set GPIO 45 to output");
	} else {
		printk(KERN_INFO "successfully set GPIO 45 to output");
	}
	msleep(5000); // sleep 5 seconds
	gpio_set_value(45, 0);	// set value for 45 (assumed already in output)
	
	return copy_from_user(virtual_device.data, bufSource, bufCount);
}


// use gpio_is_valid method to check if GPIO exists at that address
// prints kernel alerts showing result
int checkGpio(int gpioNum) {
	int checkVal = gpio_is_valid(gpioNum);
	if (checkVal < 0) {
		printk(KERN_ALERT "GPIO number %d invalid\n", gpioNum);
	} else {
		printk(KERN_INFO "GPIO number %d valid\n", gpioNum);
	}
	return checkVal;
}


MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.

/* Notes:
 -- we probably don't need a read method?


*/
