// lcd_first_draft.c 
// Requests availability of GPIO 45 and when written to will write bit to 1, wait, then 0

#include "lcd.h"

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
		printk(KERN_ALERT "lcd: Failed to allocate a major number\n");
		return ret;
	}
	printk(KERN_INFO "lcd: major number is %d\n", MAJOR(dev_num));
	printk(KERN_INFO "Use mknod /dev/%s c %d 0 for device file\n", DEVICE_NAME, MAJOR(dev_num));

	// (2) CREATE CDEV STRUCTURE, INITIALIZING CDEV
	mcdev = cdev_alloc();
	mcdev->ops = &fops;
	mcdev->owner = THIS_MODULE;

	// After creating cdev, add it to kernel
	ret = cdev_add(mcdev, dev_num, 1);
	if (ret < 0) {
		printk(KERN_ALERT "lcd: unable to add cdev to kernel\n");
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
	printk(KERN_ALERT "lcd: successfully unloaded / exited\n");
}




/* DEVICE OPEN */
// Arguments: inode reference to file on disk, struct file represents an abstract
// Functionality: check GPIO 45 valid, request GPIO 45
int device_open(struct inode *inode, struct file* filp) {
	// if not, should initialize
	if (down_interruptible(&virtual_device.sem) != 0) {
		printk(KERN_ALERT "new_char: could not lock device during open\n");
		return -1;
	}

	// check if lcd is in use
	if(gpio_request(66, "serInput") < 0) {
		printk(KERN_ALERT "GPIO number %d failed on request\n", 66);
	} else {
		printk(KERN_INFO "GPIO number %d successfully requested\n", 66);
	}

	if(gpio_request(67, "serInput") < 0) {
		printk(KERN_ALERT "GPIO number %d failed on request\n", 67);
	} else {
		printk(KERN_INFO "GPIO number %d successfully requested\n", 67);
	}

	if(gpio_request(68, "serInput") < 0) {
		printk(KERN_ALERT "GPIO number %d failed on request\n", 68);
	} else {
		printk(KERN_INFO "GPIO number %d successfully requested\n", 68);
	}
	

	int outputSet66 = gpio_direction_output(66, 1);	// sets output direction and value for 45
	int outputSet67 = gpio_direction_output(67, 1);
	int outputSet68 = gpio_direction_output(68, 1);

	if (outputSet66 < 0 || outputSet67 < 0 || outputSet68 < 0) {
		printk(KERN_ALERT "failed to set GPIOs to output");
	} else {
		printk(KERN_INFO "successfully set GPIOs to output");
	}


	// wait before attempting initialize
	msleep(500);
	
	// initialize display
	setPinsEn(12);
	setPinsEn(12);
	setPinsEn(12);
	setPinsEn(4);
	setPinsEn(4);
	setPinsEn(3);

	setPinsEn(0);
	setPinsEn(1);

	setPinsEn(0);
	setPinsEn(8);	
	
	setPinsEn(0);
	setPinsEn(7);

	setPinsEn(0);
	setPinsEn(15);

	//setPinsEn(0);
	//setPinsEn(8);
	
	printk(KERN_INFO "new_char: device opened\n");
	return 0;
}


// Called upon close
// closes device and returns access to semaphore.
int device_close(struct inode* inode, struct  file *filp) {
	up(&virtual_device.sem);
	printk(KERN_INFO "lcd, closing device\n");
	
	// remove GPIO taken status
	gpio_free(66);
	gpio_free(67);
	gpio_free(68);
	return 0;
}


// Called when user wants to get info from device file
ssize_t device_read(struct file* filp, char* bufStoreData, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "lcd: Reading from device...\n");
	return copy_to_user(bufStoreData, virtual_device.data, bufCount);
}


/* WRITE TO DEVICE */
// Called when user wants to send info to device
// sets GPIO 45 to output and high for 5 seconds, then back to low
ssize_t device_write(struct file* filp, const char* bufSource, size_t bufCount, loff_t* curOffset) {
	printk(KERN_INFO "lcd: writing to device...\n");

	int i = 0;
	int charsInLine = 0;

	while (bufSource[i] != '\0') {
		int charIn = (int) bufSource[i];

		// if we pass in "\t", do full clear and reset regardless of position
		if (charIn == (int) '\t') {
			setPinsEn(0);
			setPinsEn(8);
			charsInLine = 0;
			i++;
			continue;
		}
		

		// if we pass in "\n" and we are on the first line, go to the next line
		if ((charIn == 10) && (charsInLine < 16)) {
			setPinsEn(3);
			setPinsEn(0);
			charsInLine = 16;
		}
	
		// if we pass in "\n" and we are on the second, clear screen and reset
		else if ((charIn == 10) && (charsInLine >= 16)) {
			setPinsEn(0);
			setPinsEn(8);
			charsInLine = 0;
		}
	
		// otherwise, just print the character
		else {
			// set pins
			setPinsEn(flipASCII(charIn/16) + 64 + 32);
			setPinsEn(flipASCII(charIn % 16) + 64);

			// update the number of characters
			charsInLine++;

			// move to line 2 when first line full, and clear everything and reset when both are full
			if (charsInLine == 16) {	
				setPinsEn(3);
				setPinsEn(0);
			} else if (charsInLine == 32) {
				setPinsEn(0);
				setPinsEn(8);
				charsInLine = 0;
			}
		}
		i++;
	}
	
	return copy_from_user(virtual_device.data, bufSource, bufCount);
}


MODULE_LICENSE("GPL"); // module license: required to use some functionalities.
module_init(driver_entry); // declares which function runs on init.
module_exit(driver_exit);  // declares which function runs on exit.




// METHODS FOR WORKING WITH PINS AND INPUTS

/* This method takes a integer representing a four-bit input and returns the integer created by flipping the four bits in binary. */
int flipASCII(int x) {
	int bitOne = (x>>3)%2;
	int bitTwo = (x>>2)%2;
	int bitThree = (x>>1)%2;
	int bitFour = (x>>0)%2;
	int result = 1 * bitOne + 2 * bitTwo + 4 * bitThree + 8 * bitFour;
	printk(KERN_INFO "In flipASCII. Received: %d, returned: %d\n", x, result);
	return result;
}


/* This method takes an integer representing a three-bit input set gpio_pins 66, 67, 68 to those bit values (66 is highest-order bit, 68 is lowest-order bit. */
void setPins(int input){
	int pinValue;
	pinValue = (input>>2)%2;
	gpio_set_value(66, pinValue);

	pinValue = (input>>1)%2;
	gpio_set_value(67, pinValue);

	pinValue = (input)%2;
	gpio_set_value(68, pinValue);
}


/* This method fills the shift register, then flips the Rclock to load it. */
void sendBits(int input){
	
	//reset
	setPins(0); //00000
	msleep(1);
	setPins(2); //00010
	msleep(1);

	for (int i = 0; i <= 7; i++){
		
		//input 1 and shift
		if (((input>>i)%2)==1){
			setPins(4); // 10001
			msleep(1);
			setPins(5); // 10011
			msleep(1);
		}

		//input 0 and shift
		else { 
			setPins(0); // 00001
			msleep(1);
			setPins(1); // 00011
			msleep(1);
		}
	}

	// enable read
	setPins(0);
	msleep(1);
	setPins(2);  //testing not disabling output between data entries
	msleep(1);
	
}


/* This method send a four-bit input to the lcd: first with EN turned off, then with EN enabled, then with EN off again. */
void setPinsEn(int input){
	
	// add 2^4 = 16 to flip enable pin to 1	
	int inputPlusSixteen = input + 16;
	
	sendBits(input);
	msleep(1);
	sendBits(inputPlusSixteen);
	msleep(1);
	sendBits(input);
	msleep(1);
}
