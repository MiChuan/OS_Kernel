// syscall_64.tbl
# mycalls 
333 64  hello_world     sys_hello_world
334 64  mycopy          sys_mycopy


// syscalls.h
/* my syscalls */
asmlinkage asmlinkage long sys_hello_world(void);
asmlinkage asmlinkage long sys_mycopy(const char __user *target, const char __user *source);


// sys.c
asmlinkage asmlinkage long sys_hello_world(void)
{
	printk("mycall: hello world !");
	return 0;
}

SYSCALL_DEFINE2(mycopy, const char __user *, _target, const char __user *, _source){
    char *target;
    char *source;
    char buf[512]; 
	int sor_fd, tar_fd, Readsum, WriteRes;
	
	//get point
	target = strndup_user(_target,PAGE_SIZE);
	if(IS_ERR(target)){
	    printk("point error!\n");
	    return -1;
	}
	
	source = strndup_user(_source,PAGE_SIZE);
	if(IS_ERR(source)){
	    printk("point error!\n");
	    return -1;
	}

	// kernel -> User space    -->  fs
	mm_segment_t fs;
	fs = get_fs();  //get access restriction value
	set_fs(get_ds());  //set kernel restriction value

	// open file
	sor_fd = sys_open(source, O_RDONLY, S_IRUSR);
	if (sor_fd == -1)
	{
		printk("copy: open %s error\n", source);
		set_fs(fs);   //recovery
		return -1;
	}

	tar_fd = sys_open(target, O_WRONLY | O_CREAT, S_IRUSR | S_IWUSR);
	if (tar_fd == -1)
	{
		printk("copy: create %s error\n", target);
		set_fs(fs);   //recovery
		return -1;
	}
	printk("open file success\n");

	while (1)
	{
		Readsum = sys_read(sor_fd, buf, 512);
		if (Readsum == -1)
		{
			printk("copy: read %s error\n", source);
			set_fs(fs);   //recovery
			return -2;
		}
		else if (Readsum > 0)
		{
			WriteRes = sys_write(tar_fd, buf, Readsum);
			if (WriteRes != Readsum)
			{
				printk("copy: write %s error\n", target);
				set_fs(fs);   //recovery
				return -2;
			}
		}
		else if (Readsum == 0)
		{
			printk("copy: copy %s complete\n", source);
			break;
		}
	}

	// close file
	sys_close(tar_fd);
	sys_close(sor_fd);
	set_fs(fs);   //recovery

	return 0;
}
