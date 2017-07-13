#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h> //设备号和设备号分配
#include <linux/uaccess.h> //copy_from_user函数
#include <mach/gpio.h>  //s5pv210 的gpio端口的定义
#include <linux/device.h>
#include <linux/io.h>

/*定义端口*/
int car_port[10]={
			//电机驱动IO
				S5PV210_GPE0(3),
				S5PV210_GPE0(4),
				S5PV210_GPE0(5),
				S5PV210_GPE0(6),
				S5PV210_GPE0(7),
				S5PV210_GPE1(0),
				S5PV210_GPE1(1),
				S5PV210_GPE1(2),
			//红外驱动IO
			    S5PV210_GPH2(2),
    			S5PV210_GPH2(3),};

/*红外感应*/
int get_value(void)     
{
    int right_value = gpio_get_value(car_port[8]);
    int left_value = gpio_get_value(car_port[9]);
    return (right_value + left_value*2);
}

/*自定义的open函数*/
int car_open(struct inode *inode, struct file *pfile)
{
	int i;
    //初始化查询
    for(i = 0;i < 8;i++)
    {
        gpio_free(car_port[i]); 
		if (gpio_request(car_port[i],"car") < 0)
		{
			printk("GPIO %d request failed\n",i);
		}
    }
    for (i = 0;i < 8;i++)
    {
        gpio_direction_output(car_port[i],0);
    }
    return 0;
}

/*自定义的close函数 */
int car_close(struct inode *inode, struct file *pfile)
{
    int i;
    for(i = 0;i < 8;i++)
    {
 		gpio_set_value(car_port[i],1);
		gpio_free(car_port[i]);       
    }
    return 0;
}

//小车停止
static void stop(void)
{
	int i;
	for(i=0;i<8;i++)
		gpio_set_value(car_port[i],1);
}

//小车前进
static void run(void)
{
	int i;
	for(i=0;i<8;i+=2)
		gpio_set_value(car_port[i],0);
	for(i=1;i<8;i+=2)
		gpio_set_value(car_port[i],1);
}

//小车后退
static void back(void)
{
	int i;
	for(i=0;i<8;i+=2)
		gpio_set_value(car_port[i],1);
	for(i=1;i<8;i+=2)
		gpio_set_value(car_port[i],0);
}

//左转
static void left(void)
{
	gpio_set_value(car_port[0],1);
	gpio_set_value(car_port[1],0);
	gpio_set_value(car_port[2],1);
	gpio_set_value(car_port[3],1);
	gpio_set_value(car_port[4],1);
	gpio_set_value(car_port[5],1);
	gpio_set_value(car_port[6],0);
	gpio_set_value(car_port[7],1);
}

//右转
static void right(void)
{
	gpio_set_value(car_port[0],1);
	gpio_set_value(car_port[1],1);
	gpio_set_value(car_port[2],1);
	gpio_set_value(car_port[3],0);
	gpio_set_value(car_port[4],0);
	gpio_set_value(car_port[5],1);
	gpio_set_value(car_port[6],1);
	gpio_set_value(car_port[7],1);
}

//向后左转
static void b_left(void)
{
	gpio_set_value(car_port[0],1);
	gpio_set_value(car_port[1],0);
	gpio_set_value(car_port[2],1);
	gpio_set_value(car_port[3],1);
	gpio_set_value(car_port[4],1);
	gpio_set_value(car_port[5],1);
	gpio_set_value(car_port[6],0);
	gpio_set_value(car_port[7],1);
}

//向后右转
static void b_right(void)
{
	gpio_set_value(car_port[0],1);
	gpio_set_value(car_port[1],1);
	gpio_set_value(car_port[2],1);
	gpio_set_value(car_port[3],0);
	gpio_set_value(car_port[4],0);
	gpio_set_value(car_port[5],1);
	gpio_set_value(car_port[6],1);
	gpio_set_value(car_port[7],1);
}

//自动模式
static void auto(void)
{
	while(1)
    {
        switch(get_value())
        {
            case 0:
            {
                back();
                sleep(1);
                break;
            }
            case 1:right();break;
            case 2:left();break;
            case 3:run();break;
        }
    }
}


//car的写函数,将write设备文件的操作重新解读
ssize_t car_write(struct file *pfile, const char __user *buffer, size_t len, loff_t *loff)
{
	int data;
	copy_from_user(&data,buffer,1);
	if(data==0)
		stop();
	if(data==1)
		run();
	if(data==2)
		back();
	if(data==3)
		left();
	if(data==4)
		right();
	if(data==5)
		b_left();
	if(data==6)
		b_left();
	if(data==7)
		run(); 	
	return 1;
}

/*文件操作结构体定义,顺便初始化,这个变量在注册驱动的时候要用到*/
struct file_operations fops={
    .owner=THIS_MODULE,
    .open=car_open,//将自定义的函数赋值给文件操作结构体，实际是注册
	.release=car_close,//同上
	.write=car_write,//同上
};

dev_t devid=0;//设备号

/*初始化*/ 
static int __init car_init(void)
{
    printk(KERN_EMERG "hello world!I m a module.\n");

    // 动态分配设备号
    if(alloc_chrdev_region(&devid,250,1,"carID")==0)
    {
        printk(KERN_INFO "dev id was allocated. id is %u\n",devid);
        printk(KERN_INFO "major is %d, minor is %d\n",MAJOR(devid),MINOR(devid));
    }

    /*将驱动注册到系统里*/
    register_chrdev(MAJOR(devid),"justforshow",&fops);


    return 0;
}

/*__exit在这里表示，如果模块不允许卸载（例如编进了内核镜像里）,
那么本函数就可以直接丢弃，不编译到具体的二进制文件中。*/
static void __exit car_exit(void)
{
    printk(KERN_EMERG "Goodby cruel world!\n");
		/*释放gpio端口*/
	unregister_chrdev(MAJOR(devid),"justforshow");//驱动的反注册
}

module_init(car_init);
module_exit(car_exit);
MODULE_LICENSE("GPL");



