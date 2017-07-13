#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
	if(argc<2)
	{
		printf("use: filename NoofLed\n");
		exit(EXIT_FAILURE);
	}
	int fd=open("/dev/cardriver",O_WRONLY);
	if(fd<0)
		printf("device open failed.\n");
	int num=atoi(argv[1]);//转换成整型数
	write(fd,&num,1);	
	sleep(2);
	close(fd);
}
