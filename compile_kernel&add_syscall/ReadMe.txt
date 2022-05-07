/*****************************get_ds, set_fs, get_fs函数的使用**********************************/
get_ds获得kernel的内存访问地址范围（IA32是4GB），set_fs是设置当前的地址访问限制值，get_fs是取得当前的地址访问限制值。进程由用户态进入核态，linux进程的task_struct结构中的成员addr_limit也应该由0xBFFFFFFF变为0xFFFFFFFF(addr_limit规定了进程有用户态核内核态情况下的虚拟地址空间访问范围，在用户态，addr_limit成员值是0xBFFFFFFF也就是有3GB的虚拟内存空间，在核心态，是0xFFFFFFFF,范围扩展了1GB)。使用这三个函数是为了安全性。为了保证用户态的地址所指向空间有效，函数会做一些检查工作。
如果set_fs(KERNEL_DS),函数将跳过这些检查。
/**********************************************************************************************/
//典型用法：
#define __KERNEL_SYSCALLS__
#include <linux/unistd.h>;
#include <linux/init.h>;
#include <linux/module.h>;
#include <linux/kernel.h>;
#include <linux/file.h>;
#include <linux/fs.h>;
#include <linux/sched.h>;
#include <asm/uaccess.h>;
#include <asm/processor.h>;


int init_module(void)
{
         struct file *fp = NULL;
         char buf[100];
         int i;

         for(i=0;i<100;i++)
                 buf[i] = 0;
         printk(KERN_ALERT "Hello ,ftyjl.\n");
         fp = filp_open("/tmp/8899", 3, 0);   //内核的open函数，返回struct file *
         if (fp == NULL)
                 printk(KERN_ALERT "filp_open error ,ftyjl.\n");
                 
         /*下面两步，设置当前执行环境为kernel_ds，否则会出错*/        
         mm_segment_t old_fs=get_fs(); 
         set_fs(get_ds());
         
         fp->f_op->read(fp, buf, 2, &fp->f_pos);   //调用真正的read
         set_fs(old_fs);   //恢复环境
         printk(KERN_ALERT "ftyjl:read[%s]\n", buf);

         printk(KERN_ALERT "end of Hello ,ftyjl.\n");
         return 0;
}
void cleanup_module(void)
{
         printk(KERN_ALERT "Good bye, ftyjl\n");
}

MODULE_LICENSE("Proprietary");

/****************************************内核函数传参************************************/
1 内核函数调用系统调用，参数是由内核传入系统调用函数，而不是由用户空间，内核函数传入文件名指针char *，需要在内核函数中将文件名传入系统调用，需将文件名从用户空间拷贝进内核空间
2 https://www.cnblogs.com/arnoldlu/p/8879800.html
3 只有使用上面的方法，才能在内核中使用open,write等的系统调用。
其实这样做的主要原因是open,write的参数在用户空间，在这些系统调用的实现里需要对参数进行检查，就是检查它的参数指针地址是不是用户空间的。
系统调用本来是提供给用户空间的程序访问的，所以，对传递给它的参数（比如上面的buf、buf1），它默认会认为来自用户空间。
在vfs_write()函数中，为了保护内核空间，一般会用get_fs()得到的值来和USER_DS进行比较，从而防止用户空间程序“蓄意”破坏内核空间。
为了解决这个问题， set_fs(KERNEL_DS)将其能访问的空间限制扩大到KERNEL_DS，这样就可以在内核顺利使用系统调用了！
内核使用系统调用参数肯定是内核空间，为了不让这些系统调用检查参数所以必须设置  set_fs(KERNEL_DS)才能使用该系统调用。
vfs_write的流程可调用access_ok，而access_ok会判断访问的buf是否在0~addr_limit之间，如何是就ok；否则-EFAULT，这显然是为用户准备的检查。
addr_limit一般设为USER_DS，在内核空间，buf肯定>USER_DS，必须修改addr_limit，这就是set_fs的由来。

/*********************************************************************************/
1 open,write的参数在用户空间，在这些系统调用的实现里需要对参数进行检查，就是检查它的参数指针地址是不是用户空间的。系统调用本来是提供给用户空间的程序访问的，所以，对传递给它的参数（比如上面的buf、buf1），它默认会认为来自用户空间。在vfs_write()函数中，为了保护内核空间，一般会用get_fs()得到的值来和USER_DS进行比较，从而防止用户空间程序“蓄意”破坏内核空间。为了解决这个问题， set_fs(KERNEL_DS)将其能访问的空间限制扩大到KERNEL_DS，这样就可以在内核顺利使用系统调用了！内核使用系统调用参数肯定是内核空间，为了不让这些系统调用检查参数所以必须设置  set_fs(KERNEL_DS)才能使用该系统调用。
2 SYSCALL_DEFINEx宏
3 strndup_user 函数<util.c>从用户空间复制文件名到内核空间，copy_from_user，strncpy_from_user
<./linux/arch/x86/lib/usercopy_32.c >函数可实现从用户空间复制字符到用户空间
char *strndup_user(const char __user *s, long n)
{
	char *p;
	long length;

	length = strnlen_user(s, n);//系统调用，s为用户空间指针，检查通过，函数返回s所指字符串长度

	if (!length)
		return ERR_PTR(-EFAULT);

	if (length > n)
		return ERR_PTR(-EINVAL);

	p = memdup_user(s, length);//系统调用，从s拷贝length长度的字符到内核

	if (IS_ERR(p))  //检查指针合法性
		return p;

	p[length - 1] = '\0';

	return p;
}
4 传入参数/指针到内核函数/系统调用时，要特别检查内存合法性，IS_ERR函数进行检查
