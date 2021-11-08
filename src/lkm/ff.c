/*
 *: Filename    : is-kbd-rkt.c
 *: Date        : 2021-10-18
 *: Author      : "Kjetil Kristoffer Solberg" <post@ikjetil.no>
 *: Version     : 1.2
 *: Description : A Linux Kernel Module that produces output that detects an SMM keyboard rootkit.
*/
/*
 * #include
 */
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/ioport.h>
#include <linux/pci.h>
#include <linux/string.h>
#include <asm/msr.h>
#include <linux/proc_fs.h>
#include <linux/stddef.h>

/*
 * #define
 */
#define VERSION_NO    "1.0"
#define DEVICE_NAME   "ff"
#define CLASS_NAME    "ff"

/*
 * module metadata
 */
MODULE_AUTHOR("Kjetil Kristoffer Solberg <post@ikjetil.no>");
MODULE_DESCRIPTION("Output all binary 1's.");
MODULE_LICENSE("GPL");
MODULE_VERSION(VERSION_NO);

/*
 * function prototypes
 */
static ssize_t device_read(struct file *, char *, size_t, loff_t *);
static char *ff_devnode(struct device *dev, umode_t *mode);

/*
 * static variables
 */
static int major_num;
static struct class* char_class = NULL;
static struct device* char_device = NULL;

/*
 * device_read
 * device read function
 */
static ssize_t device_read(struct file *f, char *buffer, size_t len, loff_t *offset)
{	
	size_t l = 0;
	char ff = 0xFF;

	if (buffer == NULL) {
		return 0;
	}

	for ( l = 0; l < len; l++) {
		put_user(ff,&(buffer[l]));
	}

    return len;
}

/*
 * file_operations
 * driver file_operations structure
 */
static struct file_operations file_ops = {
	.owner = THIS_MODULE,
    .read = device_read,
};

/*
 * ff_devnode
 * set devnode mode
 */
static char *ff_devnode(struct device *dev, umode_t *mode)
{
	if (!mode)
		return NULL;
	
	// *mode must be 0664 for depmod to work. 2021-10-24
	*mode = 0664;
	
	return NULL;
}

/*
 * LKM init method
 */
static int __init ff_init(void)
{
    //
	// register char device
	//
    major_num = register_chrdev(0, DEVICE_NAME, &file_ops);
    if (major_num < 0) {
        printk(KERN_ALERT DEVICE_NAME ": error registering device: %d\n", major_num );
        return major_num;
    } 
    printk(KERN_INFO DEVICE_NAME ": lkm loaded with device major number %d\n", major_num);

    //
	// register device class
	//
    char_class = class_create(THIS_MODULE,CLASS_NAME);
    if (IS_ERR(char_class)) {
        unregister_chrdev(major_num, DEVICE_NAME);
		printk(KERN_ALERT DEVICE_NAME ": error registering device class\n");
		return PTR_ERR(char_class);
    }
    printk(KERN_INFO DEVICE_NAME ": class registered successfully\n");

	char_class->devnode = ff_devnode;

    //
	// register device driver
	//
    char_device = device_create(char_class, NULL, MKDEV(major_num, 0), NULL, DEVICE_NAME);
    if (IS_ERR(char_device)){
        class_destroy(char_class);
		unregister_chrdev(major_num, DEVICE_NAME);
		printk(KERN_ALERT DEVICE_NAME "error creating device\n");
		return PTR_ERR(char_device);
    }
    printk(KERN_INFO DEVICE_NAME ": device class created correctly\n");
    
	//
	// return success
	//
	return 0;
}

/*
 * LKM exit method
 */
static void __exit ff_exit(void)
{
	device_destroy(char_class, MKDEV(major_num, 0));
    class_unregister(char_class);
    unregister_chrdev(major_num, DEVICE_NAME);

    printk(KERN_INFO DEVICE_NAME ": module unloaded\n");
}

/*
 * LKM module init and exit
 */
module_init(ff_init);
module_exit(ff_exit);
