#include <linux/init.h>
#include <linux/module.h>

static int __init hello_init(void)
{
  printk("hello kernel\n");
  return 0;
}
module_init(hello_init);

static void __exit hello_exit(void)
{
  printk("bye kernel\n");
}
module_exit(hello_exit);

MODULE_LICENSE("GPL v2"); // 开源许可证
MODULE_DESCRIPTION("hello module for RPi 3B+"); // 模块描述
MODULE_ALIAS("Hello"); // 模块别名
MODULE_AUTHOR("Philon"); // 模块作者