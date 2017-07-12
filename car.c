#include "define_head_file.h"

int main(int argc,char const *argv[])
{
	//int fd=open("/dev/cardriver",O_WRONLY);
	//if(fd<0)
		//printf("device open failed.\n");
	printf("searching for server\n");
	int client_fd=socket(AF_INET,SOCK_STREAM,0);
	if(client_fd==-1)
	{
		perror("soket fail\n");
		exit(-1);
	}
	//定义服务端地址结构
	struct  sockaddr_in  ser_addr;
	ser_addr.sin_family=AF_INET;
	ser_addr.sin_port=htons(6666);
	//ser_addr.sin_addr.s_addr=INADDR_ANY;
	ser_addr.sin_addr.s_addr=inet_addr("47.95.9.185");
	//客户端发送链接请求
	if(connect(client_fd,(struct sockaddr*)&ser_addr,sizeof(ser_addr))==-1)
	{
		printf("Connect to the server failed\n");
		exit(-1);
	}
	printf("The server has been successfully connected\n");
	char ID[4]="2";
	send(client_fd,ID,strlen(ID),0);
	char buf[1024];
	while(1)
	{
    	bzero(buf,sizeof(buf));
    	recv(client_fd,buf,sizeof(buf),0);
    	printf("Server command %s has been received\n",buf);
    	//int num=atoi(buf);//转换成整型数
    	//write(fd,&num,1);
    }
}


