#include <linux/init.h>
#include <linux/module.h>
#include <linux/cdev.h>
#include <linux/fs.h> // file_operations
#include <linux/errno.h>
#include <linux/slab.h> // kmalloc
#include <linux/uaccess.h> // copy_from/to_user

#define MEME_DEFAULT_DATA_SIZE  0x100

// meme 全局内存数据，相当与一个小u盘
// 任何应用程序都可以往它内部写东西
static struct {
    char* data; // 数据空间，之后动态分配
    int size; // 空间大小
} meme = {NULL, MEME_DEFAULT_DATA_SIZE};

// 对应用户层read读取函数，通常用户接收设备内容，如串口收数据
static ssize_t meme_read(struct file* filp, char __user *buf, size_t size, loff_t* off)
{
    // 如果当前文件偏移量已经超出内存大小，直接返回错误
    if (*off >= meme.size) {
        return -ENOMEM;
    }

    // 如果当前可访问内存长度小于要读取的长度，仅读取可访问长度
    if ((*off + size) > meme.size) {
        size = meme.size - *off;
    }

    // 拷贝内核数据到用户空间
    if (copy_to_user(buf, filp->private_data + *off, size)) {
        return -EFAULT;
    }

    *off += size;
    return size;
}

// 对应用户层write写入函数，通常用于写入设备内容，如串口发数据
static ssize_t meme_write(struct file* filp, const char __user *buf, size_t size, loff_t* off)
{
    // 如果文件偏移超出内存长度，就没必要再写了
    if (*off >= meme.size) {
        return 0;
    }

    // 如果要写入的数据长度比剩余可访问的内存长度还要大，仅写入可访问的内存长度
    if (*off + size > meme.size) {
        size = meme.size - *off;
    }

    // 拷贝用户数据到内核空间
    if (copy_from_user(filp->private_data + *off, buf, size)) {
        return -EFAULT;
    }

    *off += size;
    return size;
}

// 对应用户层open打开函数，就是打开设备描述符
static int meme_open(struct inode* inode, struct file* filp)
{
    // 如果是第一次访问设备节点，分配内存
    if (meme.data == NULL) {
        meme.data = kmalloc(MEME_DEFAULT_DATA_SIZE, GFP_KERNEL);
    }

    filp->private_data = meme.data;
    
    return 0;
}

// 对应用户层close关闭函数，就是关闭设备文件
static int meme_close(struct inode* inode, struct file* filp)
{
    // nothing todo
    return 0;
}

// 文件操作结构体，表示内核与用户层的函数操作对应关系
static const struct file_operations fops = {
    .owner = THIS_MODULE, // 这其实是个结构体，比如THIS_MODULE->name
    .read = meme_read,
    .write = meme_write,
    .open = meme_open,
    .release = meme_close,
};

// 字符设备结构体，该字符设备的核心数据表示
static struct cdev cdev;

// 驱动的主设备编号，每种类型的设备都有唯一编号
static dev_t devno = 0;

static int __init meme_init(void)
{
    // 1. 向内核申请分配一个主设备号，此设备号从0开始，分配1个，名称为meme
    if (alloc_chrdev_region(&devno, 0, 1, "meme") < 0 ) {
        printk(KERN_ERR"fail init\n");
        return (-1);
    }

    // 2. 初始化cdev结构体，并与fops文件操作之间建立联系
    cdev_init(&cdev, &fops);

    // 3. 正式向内核注册一个字符设备
    cdev_add(&cdev, devno, 1);

    printk(KERN_ALERT"meme init: %d:%d\n", MAJOR(devno), MINOR(devno));
    return 0;
}
module_init(meme_init);

static void __exit meme_exit(void)
{
    // 1. 向内核注销该字符设备
    cdev_del(&cdev);

    // 2. 向内核申请释放该设备号
    unregister_chrdev_region(devno, 1);

    printk("meme free\n");
}
module_exit(meme_exit);

MODULE_LICENSE("GPL v2");
MODULE_ALIAS("MyMemery");
MODULE_AUTHOR("Philon");
MODULE_DESCRIPTION("A character device driver");