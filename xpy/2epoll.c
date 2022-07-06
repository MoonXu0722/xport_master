#include <stdio.h>
#include <sys/epoll.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <fcntl.h>

#define _MAXFD_ 10 

typedef struct fd_buf{
	int fd;
	char *buf;
}_buf,*ptr_buf;

int set_non_block(int fd)
{
	int old_flag=fcntl(fd,F_GETFL);
	if(old_flag<0){
		perror("fcntl");
		exit(-4);
	}
	if(fcntl(fd,F_SETFL,old_flag|O_NONBLOCK)<0){
		perror("fcntl");
		return -1;
	}

	return 0;

}

int write_fd(ptr_buf fd_buf,int num,int epoll_fd,struct epoll_event* event)
{
		int fd=fd_buf->fd;
		ssize_t size=0;

		while(1){
			size=write(fd,fd_buf->buf,strlen(fd_buf->buf));
			if(size<0&&errno!=EAGAIN){	
				perror("write");
				return -1;
    	 	}
			if(errno==EAGAIN){
		//	printf("no memory to write!\n");
			return -2;
			}
		}
		return fd;

}

ptr_buf read_fd(int epoll_fd,int fd,ptr_buf buf,struct epoll_event* event,int num)
{	
	if(	set_non_block(fd)<0){
		
		return NULL;
	}
	
	buf->fd=fd;
	buf->buf=(char*)malloc(sizeof(char)*1024);
	ssize_t size;
	int _num=0;
		
	size=read(fd,buf->buf+_num,1024);
//	printf("size %d\n",size);
	if(size<0){
		perror("read");
		return NULL;
	}
	if(size==0){
		struct epoll_event ev;
		ev.data.fd=fd;
		close(fd);
		epoll_ctl(epoll_fd,EPOLL_CTL_DEL,fd,event);
		printf("client [%d] close...\n",fd);
		free(buf->buf);
		free(buf);
		return NULL;
	}
	_num+=size;
	while(size=read(fd,buf->buf+_num,1024)>0){
		if(errno==EAGAIN){
			break;
		}
		_num+=size;
	}
	if(_num>1023){
		printf("client [%d]'s buf overflow!...",fd);
		buf->buf[1023]=0;
	}
	else
		buf->buf[_num]=0;
	printf("client [%d]# %s\n",fd,buf->buf);
	return buf;
}

int startup(char* ip,int port)
{
	int sock=socket(AF_INET,SOCK_STREAM,0);
	struct sockaddr_in server;
	server.sin_family=AF_INET;
	server.sin_addr.s_addr=inet_addr(ip);
	server.sin_port=htons(port);
//	printf("port  %d  %d",port,(htons(port)));
	if(bind(sock,(struct sockaddr *)&server,sizeof(server))<0){
		perror("bind");
		exit(-2);
	}
	if(listen(sock,5)<0){
		perror("listen");
		exit(-3);
	}
	return sock;
}

void usage(char* arg)
{
	printf("usage %s [ip] [port]\n",arg);

}

void epollup(int sock)
{	
	int epoll_fd=epoll_create(256);
	if(epoll_fd<0){
		perror("epoll");
		return;
	}
	int timeout_num=0;
	int done=0;
	int timeout=5000;
	int i=0;
	int ret_num=-1;

	struct epoll_event ev;
	struct epoll_event event[10];
	ev.data.fd=sock;
	ev.events=EPOLLIN|EPOLLET;
//	fd_num=1;
//	printf("listen sock%d\n",sock);
	if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,sock,&ev)<0){
		perror("epoll_ctl");
		return ;
	}
	while(!done){
		switch(ret_num=epoll_wait(epoll_fd,event,256,timeout)){
		case -1:{
			perror("epoll_wait");	
			break;
		}
		case 0 :{
			if(	timeout_num++>5)
				done=1;
			printf("time out...\n");
			break;
		}
		default:{
				for(i=0;i<ret_num;++i){
					if(event[i].data.fd==sock&&event[i].events&EPOLLIN){
						int new_sock=-1;
						struct sockaddr_in client;
						socklen_t len=sizeof(client);
						if((new_sock=accept(sock,(struct sockaddr*)&client,&len))<0){
							perror("accept");
							printf("%s : %d \n",strerror(errno),new_sock);
							continue;
						}
						//printf("%s\n",strerror(errno));			
	//					printf(" epoll accept  %d\n",new_sock);
						ev.data.fd=new_sock;
						ev.events=EPOLLIN|EPOLLET;
						if(epoll_ctl(epoll_fd,EPOLL_CTL_ADD,new_sock,&ev)<0){
							perror("epoll_ctl");
							return ;
						}
//						fd_num++;
						printf("get a connect [%d]...\n",new_sock);
					}
					else {
						if(event[i].events&EPOLLIN){
	//						printf("event read!\n");
							int fd=event[i].data.fd;
							ptr_buf buf=(ptr_buf)malloc(sizeof(_buf));
							if(read_fd(epoll_fd,fd,buf,event,i)==NULL)
								continue;
							event[i].data.ptr=buf;
							event[i].events=EPOLLOUT;
						}
						if(event[i].events&EPOLLOUT){
							int fd=-1;
							if(fd=write_fd(event[i].data.ptr,i,epoll_fd,event)<0){
								continue;
							}
							event[i].data.fd=fd;
							event[i].events=EPOLLIN|EPOLLET;
						}
						
					}
				}
			break;
		}
		
		}
	}
}

int main(int argc,char* argv[]){
	if(argc!=3){
		usage(argv[0]);
		exit(-1);
	}	
	int port=atoi(argv[2]);
//	printf("port %s %d",argv[2],port);
	
	int listen_sock=startup( argv[1],port);
 //   set_non_block(listen_sock);
	epollup(listen_sock);
	close(listen_sock);
	return 0;
}
