#include <linux/device.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/mutex.h>
#include <linux/uaccess.h>

#define CLASS_NAME    "pa2_class"
#define DEVICE_NAME   "pa2"
#define BUFFER_LENGTH 1024

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Derya Hancock <deryahancock@knights.ucf.edu>, Jerasimos Strakosha <jstrakosha@knights.ucf.edu>, Richard Zarth <rlziii@knights.ucf.edu>");
MODULE_DESCRIPTION("A simple character-mode device driver");
MODULE_VERSION("1.0");

/* FUNCTION PROTOTYPES */
int int_module(void);
void cleanup_module(void);
static int dev_open(struct inode *, struct file *);
static ssize_t dev_read(struct file *, char *, size_t, loff_t *);
static ssize_t dev_write(struct file *, const char *, size_t, loff_t *);
static int dev_release(struct inode *, struct file *);

/* GLOBAL VARIABLES */
static int majorNumber;
static int numberOfOpens = 0;
static char message[BUFFER_LENGTH] = {0};
static char receivedMessage[BUFFER_LENGTH] = {0};
static short messageSize;
static struct class *pa2Class = NULL;
static struct device *pa2Device = NULL;
static DEFINE_MUTEX(pa2_mutex);

static struct file_operations fops =
{
    .open    = dev_open,
    .read    = dev_read,
    .write   = dev_write,
    .release = dev_release,
};

int init_module(void)
{
    printk(KERN_INFO "PA2: Initializing module.\n");

    majorNumber = register_chrdev(0, DEVICE_NAME, &fops);

    if (majorNumber < 0)
    {
        printk(KERN_ALERT "PA2: Failed to register a major number.\n");

        return majorNumber;
    }

    printk(KERN_INFO "PA2: Registered with major number %d.\n", majorNumber);

    pa2Class = class_create(THIS_MODULE, CLASS_NAME);

    if (IS_ERR(pa2Class))
    {
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PA2: Failed to register a class.\n");

        return PTR_ERR(pa2Class);
    }

    printk(KERN_INFO "PA2: Device class registered.\n");

    pa2Device = device_create(pa2Class, NULL, MKDEV(majorNumber, 0), NULL, DEVICE_NAME);

    if (IS_ERR(pa2Device))
    {
        class_destroy(pa2Class);
        unregister_chrdev(majorNumber, DEVICE_NAME);
        printk(KERN_ALERT "PA2: Failed to create device.\n");

        return PTR_ERR(pa2Device);
    }

    mutex_init(&pa2_mutex);

    printk(KERN_INFO "PA2: Device created successfully.\n");

    return 0;
}

void cleanup_module(void)
{
    mutex_destroy(&pa2_mutex);

    device_destroy(pa2Class, MKDEV(majorNumber, 0));
    class_unregister(pa2Class);
    class_destroy(pa2Class);
    unregister_chrdev(majorNumber, DEVICE_NAME);

    printk(KERN_INFO "PA2: Removing module.\n");
}

static int dev_open(struct inode *inodep, struct file *filep)
{
    printk(KERN_INFO "\nPA2: OPEN Full string: %s\n", message);

    if (!mutex_trylock(&pa2_mutex))
    {
        printk(KERN_ALERT "PA2: Device already in use by another process.\n");

        return -EBUSY;
    }

    numberOfOpens++;

    printk(KERN_INFO "PA2: Device has been opened %d time(s).\n", numberOfOpens);

    return 0;
}

static ssize_t dev_read(struct file *filep, char *buffer, size_t len, loff_t *offset)
{
    int errorCount = 0;
    int i=0;
    int stringLen=len;
    int off = *offset;

    printk(KERN_INFO "\nPA2: READ Full string: %s\n", message);

    // TODO: Update this to also accept an offset?

    // If the requested read length is more than the available space
    // then reduce the read length to the maximum available
    // else use the requested read length
    //NOT SURE ABOUT THIS LINE
    stringLen = BUFFER_LENGTH < len ? BUFFER_LENGTH : len;
    //len = strlen(message) < len ? strlen(message) : len;
    errorCount = copy_to_user(buffer, message, len);
     while (stringLen>0)
     {
        buffer[i]= message[i+off];
        stringLen--;

     }

    //strcpy(message, &message[len]);
    // snprintf(message, len, "%s", &message[len]);
    // message[len] = '\0';

    if (errorCount == 0)
    {
        printk(KERN_INFO "PA2: Sent %d characters to the user.\n", len);

        printk(KERN_INFO "PA2: READ Full string: %s\n", message);

        return len;
    } else {
        printk(KERN_INFO "PA2: Failed to send %d characters to the user.\n", errorCount);

        return -EFAULT;
    }
}

static ssize_t dev_write(struct file *filep, const char *buffer, size_t len, loff_t *offset)
{
    int errorCount = 0;

    printk(KERN_INFO "\nPA2: WRITE Full string: %s\n", message);

    // If the requested write length is more than the available space
    // then reduce the write length to the maximum available
    // else use the requested write length
    len = (BUFFER_LENGTH - strlen(message)) < len ? (BUFFER_LENGTH - strlen(message)) : len;

    errorCount = copy_from_user(receivedMessage, buffer, len);

    snprintf(message, (strlen(message) + len + 1), "%s%s", message, receivedMessage);
    messageSize = strlen(message);
    printk(KERN_INFO "PA2: Received %zu characters from the user.\n", len);

    printk(KERN_INFO "PA2: WRITE Full string: %s\n", message);

    return len;
}

static int dev_release(struct inode *inodep, struct file *filep)
{
    mutex_unlock(&pa2_mutex);

    printk(KERN_INFO "\nPA2: RELEASE Full string: %s\n", message);

    printk(KERN_INFO "PA2: Device successfully closed.\n");

    return 0;
}
