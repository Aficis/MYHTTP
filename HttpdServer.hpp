#ifndef __HTTPD_SERVER_HPP__
#define __HTTPD_SERVER_HPP__

#include <pthread.h>
#include "ProtoUtil.hpp"

#define MAXCON 10

class HttpdServer
{
	private:
		int listen_sock;
		int port;
	public:
		HttpdServer(int port_):port(port_),listen_sock(-1)
		{}
		
		void InitServer()
		{
			listen_sock = socket(AF_INET, SOCK_STREAM, 0);
			if(listen_sock < 0){
				LOG(ERROR, "create socket error!");
				exit(2);
			}
			int opt_ = 1;
			setsockopt(listen_sock, SOL_SOCKET, SO_REUSEADDR, &opt_, sizeof(opt_));
			struct sockaddr_in local_;
			local_.sin_family = AF_INET;
			local_.sin_port = htons(port);
			local_.sin_addr.s_addr = INADDR_ANY;
			if(bind(listen_sock, (sockaddr*)&local_, sizeof(local_)) < 0){
				LOG(ERROR, "bind socket error!");
				exit(3);
			}
			if(listen(listen_sock, MAXCON) < 0){
				LOG(ERROR, "listen socket error!");
				exit(4);
			}
			LOG(INFO, "initsever succeed!");
		}
		void Start()
		{
			LOG(INFO, "starting server!");
			while(1){
				struct sockaddr_in peer_;
				socklen_t len_ = sizeof(peer_);
				int sock_ = accept(listen_sock, (sockaddr*)&peer_, &len_);
				if(sock_ < 0){
					LOG(WARNING, "accpet error!");
					continue;
				}
				LOG(INFO, "get new client! create thread handler request...");
				pthread_t tid_;
				int *sockp = new int;
				*sockp = sock_;
				pthread_create(&tid_, NULL, Entry::HandlerRequest, (void*)sockp);
			}
		}

		~HttpdServer()
		{
			if(listen_sock != -1){
				close(listen_sock);
			}
			port = -1;
		}
};

#endif









