#ifndef HTTP_CONN_H
#define HTTP_CONN_H

#include <sys/types.h>
#include <sys/uio.h>     // readv/writev
#include <arpa/inet.h>   // sockaddr_in
#include <stdlib.h>      // atoi()
#include <errno.h>      

#include "log.h"
#include "buffer.h"
#include "httprequest.h"
#include "httpresponse.h"
/*
进行读写数据并调用httprequest 来解析数据以及httpresponse来生成响应
*/
class HttpConn {
public:
    HttpConn();
    ~HttpConn();
    
    void init(int sockFd, const sockaddr_in& addr);  //初始化一个连接
    ssize_t read(int* saveErrno);                    //从socket读数据到读缓冲区中
    ssize_t write(int* saveErrno);                   //将http响应报文写人socket
    void Close();                                    //关闭一个连接
    int getFd() const;                               //获取连接的文件描述符
    int getPort() const;                             //获取连接的端口号
    const char* getIP() const;                       //获取连接的IP
    sockaddr_in getAddr() const;                     //获取连接的地址结构体
    bool process(ConnectionPool* pool);                                  //处理请求过程封装    

    // 写的总长度
    int ToWriteBytes() { 
        return iov_[0].iov_len + iov_[1].iov_len; 
    }

    bool IsKeepAlive() const {
        return request_.IsKeepAlive();
    }

    static bool isET;
    static const char* srcDir;
    static std::atomic<int> userCount;  // 原子，支持锁
    
private:
   
    int fd_;
    struct  sockaddr_in addr_;

    bool isClose_;
    
    int iovCnt_;
    struct iovec iov_[2];
    
    Buffer readBuff_; // 读缓冲区
    Buffer writeBuff_; // 写缓冲区

    HttpRequest request_;
    HttpResponse response_;
};

#endif
