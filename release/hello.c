#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>

static int custom_init(void) {

    printk("Hello World!\n");
#ifdef DEBUG
    printk("We are in Debug release\n");
#endif
    return 0;
}

static void custom_exit(void) {

    printk("Goodbye World!\n");
}

module_init(custom_init);
module_exit(custom_exit);

MODULE_LICENSE("GPL");
