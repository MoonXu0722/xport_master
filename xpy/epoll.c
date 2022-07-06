#include <sys/epoll.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <arpa/inet.h>
#include <string.h>



int main(){
	struct sockaddr_in serv, cli;
	int fd =socket(AF_INET, SOCK_STREAM, 0);
	int epfd = epoll_create(256);
	struct epoll_event ev, events[20];
	ev.data.fd = fd;
	ev.events = EPOLLIN|EPOLLET;
	epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev);
	serv.sin_family = AF_INET;
	serv.sin_port = htons(9999);
	serv.sin_addr.s_addr = htonl(INADDR_ANY);
	bind(fd, (struct sockaddr*)&serv, sizeof(serv));
	listen(fd, 100);
	char line[4096];
	while (1) {
		int nfds = epoll_wait(epfd, events, 100, -1);
		switch(nfds) {
			case -1:
				perror("epoll_wait");
				return -1;
			case 0: 
				printf("timeout\n");
			default:
				for (int i=0; i<nfds; i++) {
					if (events[i].data.fd == fd && events[i].events&EPOLLIN) {
						int len = sizeof(cli);
						int connfd = accept(fd, (struct sockaddr*)&cli, &len);
						if(connfd < 0){
							return -1;
						}
						printf("accept a connection %s %d\n", inet_ntoa(cli.sin_addr), ntohs(cli.sin_port));
						ev.data.fd = connfd;
						ev.events = EPOLLIN|EPOLLET;
						epoll_ctl(epfd, EPOLL_CTL_ADD, connfd, &ev);
					} else if(events[i].events & EPOLLIN) {
						printf("epollin\n");
						memset(line, 0, sizeof(line));
						int r = read(events[i].data.fd, line, 4096);
						line[r] = 0;
						if (r>0)
							printf("client[%d]:%s\n",events[i].data.fd,  line);
						ev.data.fd = events[i].data.fd;
						ev.events = EPOLLOUT;
					} else if(events[i].events&EPOLLOUT == EPOLLOUT){
						write(events[i].data.fd, line, 4096);	
						ev.data.fd = events[i].data.fd;
						ev.events = EPOLLIN|EPOLLOUT;
						epoll_ctl(epfd, EPOLL_CTL_MOD, events[i].data.fd, &ev);
					}
				}
		}
	}
}



