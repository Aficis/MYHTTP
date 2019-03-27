/*
*GET在URL部分传参
*POST在正文部分传参
* 
*/

#ifndef __PROTO_UTIL_HPP__
#define __PROTO_UTIL_HPP__

#include <iostream>
#include <string>
#include <unistd.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sstream>
#include <strings.h>
#include "Log.hpp"
using namespace std;

#define NOT_FOUND 404
#define OK 200

#define WEB_ROOT "wwwroot"
#define HOME_PAGE "index.html"

//HTTP的四部分
class Request
{
	public:
		string rq_line;
		string rq_head;
		string blank;
		string rq_text;
	private:
		string method;
		string uri;
		string version;
		bool cgi;//method=POST,GET时uri中有？
		string path;
		string param;
	public:
		Request():blank("\n"),cgi(false)
		{}
		void RequestLineParse()
		{
			//请求解析
			stringstream ss(rq_line);
			ss >> method >> uri >> version;
		}
		void UriParse()
		{
			size_t pos_ = uri.find('?');
			if(string::npos != pos_){
				cgi = true;
				path = uri.substr(0, pos);
				param = uri.substr(pos+1);
			}else{
				path += uri;
			}
			if(path[path.size()-1]=='/'){
				path += HOME_PAGE;
			}

		}
		bool IsMethodLegal()
		{
			if(strcasecmp(method,"GET")==0 || cgi=(strcasecmp(method,"POST")==0))
				return true;
			return false;
		}
		~Request()
		{}
};

class Response
{
	private:
		string rsp_line;
		string rsp_head;
		string blank;
		string rsp_text;
	public:
		int code;
	public:
		Response():blank("\n"),code(OK)
		{}
		~Response()
		{}
};

class Connect
{
	private:
		int sock;
	public:
		Connect(int sock_):sock(sock_)
		{}
		int RecvOneLine(std::string &line_)
		{
			char c = 'X';
			while(c != '\n'){
				ssize_t s = recv(sock, &c, 1, 0);
				if(s > 0){
					if(c == '\r'){
						recv(sock, &c, 1, MSG_PEEK);
						if(c == '\n'){
							recv(sock, &c, 1, 0);
						}else{
							c = '\n';
						}
					}
					line_.push_back(c);
				}else
					break;
			}
			return line_.size();
		}
		~Connect()
		{
			if(sock >= 0){
				close(sock);
			}
		}
};

class Entry
{
	public:
		static void *HandlerRequest(void *arg_)
		{
			int sock_ = *(int*)arg_;
			delete (int*)arg_;
			Connect *con_ = new Connect(sock_);
			Request *rq_ = new Request();
			Response *rsp_ = new Response();
			
			int &code_ = rsp_->code;
			con_->RecvOneLine(rq_->rq_line);
			rq_->RequestLineParse();
			if(!rq_->IsMethodLegal()){
				code_ = NOT_FOUND;
				goto end;
			}
			rq_->UriParse();

end:
			if(code!=OK){
				
			}
			delete con_;
			delete rq_;
			delete rsp_;
		}
};

#endif







