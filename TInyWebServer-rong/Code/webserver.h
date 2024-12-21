#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <unordered_map>
#include <fcntl.h>       // fcntl()
#include <unistd.h>      // close()
#include <assert.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "epoller.h"
#include "heaptimer.h"

#include "log.h"
#include "conpool.h"
#include "threadpool.h"

#include "httpconn.h"

class WebServer {
public:
    WebServer(int port, int trigMode, int timeoutMS, bool OptLinger, 
              unsigned short sqlPort, std::string sqlUser, std::string sqlPwd, 
              std::string dbName, unsigned int initConns, unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout,
              int threadNum,
              bool openLog, int logLevel, int logQueSize);

    ~WebServer();
    void Start();

private:
    bool InitSocket_(); 
    void InitEventMode_(int trigMode);
    void AddClient_(int fd, sockaddr_in addr);
  
    void DealListen_();
    void DealWrite_(HttpConn* client);
    void DealRead_(HttpConn* client);

    void SendError_(int fd, const char*info);
    void ExtentTime_(HttpConn* client);
    void CloseConn_(HttpConn* client);

    void OnRead_(HttpConn* client);
    void OnWrite_(HttpConn* client);
    void OnProcess(HttpConn* client);

    

    static int SetFdNonblock(int fd);

    static const int MAX_FD = 65536;    // 最大连接数

    int port_;              // 端口号
    bool openLinger_;       // 关闭连接
    int timeoutMS_;         // 超时时间
    bool isClose_;          // 运行状态
    int listenFd_;          // 监听套接字
    const char* srcDir_;    // 静态资源路径
    
    uint32_t listenEvent_;  // 监听事件
    uint32_t connEvent_;    // 连接事件
   
    ThreadPool* threadpool_;    // 线程池
    ConnectionPool* connPool_;  // 数据库连接池
    std::unique_ptr<HeapTimer> timer_;          // 定时器
    std::unique_ptr<Epoller> epoller_;          // epoll
    std::unordered_map<int, HttpConn> users_;   // 连接信息
};

#endif //WEBSERVER_H
