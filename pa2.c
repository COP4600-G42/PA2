#include <linux/module.h>
#include <linux/kernel.h>

int init_module(void)
{
    printk(KERN_INFO "Installing PA2 module.\n");

    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "Removing PA2 module.\n");
}
