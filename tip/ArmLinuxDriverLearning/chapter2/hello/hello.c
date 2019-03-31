#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
    printk("HELLO_MODULE: hello world\n");
    return 0;
}

static void __exit hello_exit(void)
{
    printk("HELLO_MODULE: goodbye\n");
}

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("my first kernel module");
MODULE_ALIAS("HelloWorld");
MODULE_AUTHOR("Philon");