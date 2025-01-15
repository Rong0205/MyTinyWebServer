#include "webserver.h"

WebServer::WebServer(
            int port, int trigMode, int timeoutMS, bool OptLinger, 
            unsigned short sqlPort, std::string sqlUser, std::string sqlPwd, 
            std::string dbName, unsigned int initConns, unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout,
            int threadNum,
            bool openLog, int logLevel, int logQueSize)
            : port_(port), openLinger_(OptLinger), timeoutMS_(timeoutMS), isClose_(false)
            , timer_(new HeapTimer()), epoller_(new Epoller())
{
    //设置网页资源路径  
    srcDir_ = "/home/rong/MyTinyWebServer/TInyWebServer-rong/resources";
    HttpConn::userCount = 0; 
    HttpConn::srcDir = srcDir_;

    //初始化连接组件
    threadpool_ = ThreadPool::getThreadPool(threadNum);
    connPool_ = ConnectionPool::getConnectionPool();
    connPool_->init("127.0.0.1",sqlPort, sqlUser, sqlPwd, dbName, initConns, maxConns, maxIdleTime, connectTimeout);
    InitEventMode_(trigMode);
    if(!InitSocket_()) { isClose_ = true; }

    //初始化日志
    if(openLog){
        Log::Instance()->init(logLevel, "../testlog", ".log", 0);
        if(isClose_) { LOG_ERROR( "========== Server init error! ==========" ); }
        else{
            LOG_INFO( "========== Server init ==========");
            LOG_INFO( "Port: %d, OpenLinger: %s", port_, OptLinger ? "true" : "false" );
            LOG_INFO( "Listen Mode: %s, OpenConn Mode: %s",
                            (listenEvent_ & EPOLLET ? "ET" : "LT"),
                            (connEvent_ & EPOLLET ? "ET" : "LT") );
            LOG_INFO( "LOGSys level: %d", logLevel);
            LOG_INFO( "srcDir: %s", srcDir_);
            LOG_INFO( "SqlConn init nums: %d, ThreadPool nums: %d", initConns, threadNum);
        }
    }
}

WebServer::~WebServer(){
    close(listenFd_);
    isClose_ = true;
    //delete connPool_;
    //delete threadpool_;

}

void WebServer::InitEventMode_(int trigMode){
    listenEvent_ = EPOLLRDHUP;
    connEvent_ = EPOLLONESHOT | EPOLLRDHUP;
    switch(trigMode)
    {
    case 0:
        break;
    case 1:
        connEvent_ |= EPOLLET;
        break;
    case 2:
        listenEvent_ |= EPOLLET;
        break;
    case 3:
        listenEvent_ |= EPOLLET;
        connEvent_ |= EPOLLET;
        break;
    default:
        listenEvent_ |= EPOLLET;
        connEvent_ |= EPOLLET;
        break;
    }
    HttpConn::isET = (connEvent_ & EPOLLET);
}

bool WebServer::InitSocket_() {
    int ret;
    struct sockaddr_in addr;
    if(port_ > 65535 || port_ < 1024) {
        LOG_ERROR("Port:%d error!",  port_);
        return false;
    }
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port_);

    // 优雅关闭
    {
    struct linger optLinger = { 0 };
    if(openLinger_) {
        /* 优雅关闭: 直到所剩数据发送完毕或超时 */
        optLinger.l_onoff = 1;
        optLinger.l_linger = 1;
    }

    listenFd_ = socket(AF_INET, SOCK_STREAM, 0);
    if(listenFd_ < 0) {
        LOG_ERROR("Create socket error!", port_);
        return false;
    }

    ret = setsockopt(listenFd_, SOL_SOCKET, SO_LINGER, &optLinger, sizeof(optLinger));
    if(ret < 0) {
        close(listenFd_);
        LOG_ERROR("Init linger error!", port_);
        return false;
    }
    }

    int optval = 1;
    /* 端口复用 */
    /* 只有最后一个套接字会正常接收数据。 */
    ret = setsockopt(listenFd_, SOL_SOCKET, SO_REUSEADDR, (const void*)&optval, sizeof(int));
    if(ret == -1) {
        LOG_ERROR("set socket setsockopt error !");
        close(listenFd_);
        return false;
    }

    // 绑定
    ret = bind(listenFd_, (struct sockaddr *)&addr, sizeof(addr));
    if(ret < 0) {
        LOG_ERROR("Bind Port:%d error!", port_);
        close(listenFd_);
        return false;
    }

    // 监听
    ret = listen(listenFd_, 6);
    if(ret < 0) {
        LOG_ERROR("Listen port:%d error!", port_);
        close(listenFd_);
        return false;
    }
    ret = epoller_->AddFd(listenFd_,  listenEvent_ | EPOLLIN);  // 将监听套接字加入epoller
    if(ret == 0) {
        LOG_ERROR("Add listen error!");
        close(listenFd_);
        return false;
    }
    SetFdNonblock(listenFd_);   
    LOG_INFO("Server port:%d", port_);
    return true;
}

// 设置非阻塞
int WebServer::SetFdNonblock(int fd) {
    assert(fd > 0);
    return fcntl(fd, F_SETFL, fcntl(fd, F_GETFD, 0) | O_NONBLOCK);
}

void WebServer::SendError_(int fd, const char*info){
    int ret = send(fd, info, strlen(info), 0);
    if(ret < 0){
        LOG_ERROR("send error to client[%d] error!", fd);
    }
    close(fd);
}

void WebServer::AddClient_(int fd, sockaddr_in addr){
    users_[fd].init(fd, addr);
    if(timeoutMS_>0){
        timer_->add(fd, timeoutMS_, std::bind(&WebServer::CloseConn_, this, &users_[fd]));
    }
    epoller_->AddFd(fd, EPOLLIN | connEvent_);
    SetFdNonblock(fd);
    LOG_INFO("Client[%d] in!", users_[fd].getFd());
}

void WebServer::DealListen_(){
    struct sockaddr_in addr;
    socklen_t addrLen = sizeof(addr);
    do{
        int fd = accept(listenFd_, (struct sockaddr*)&addr, &addrLen);
        if(fd < 0){
            //LOG_ERROR("accept error!");
            break;
        }
        if(HttpConn::userCount >= MAX_FD){
            SendError_(fd, "Internal server busy");
            LOG_WARN("Client is full!");
            return;
        }
        AddClient_(fd, addr);
    }while(listenEvent_ & EPOLLET);
}

void WebServer::CloseConn_(HttpConn* client){
    LOG_INFO("Client[%d] quit!", client->getFd());
    epoller_->DelFd(client->getFd());
    client->Close();
}

void WebServer::DealWrite_(HttpConn* client){
    ExtentTime_(client);
    threadpool_->AddTask(&WebServer::OnWrite_,this, client);
}

void WebServer::DealRead_(HttpConn* client){
    ExtentTime_(client);
    threadpool_->AddTask(&WebServer::OnRead_,this, client);
}


void WebServer::ExtentTime_(HttpConn* client){
    if(timeoutMS_>0){
        timer_->adjust(client->getFd(), timeoutMS_);
    }
}


void WebServer::OnRead_(HttpConn* client){
    int ret = -1;
    int readErrno = 0;
    ret = client->read(&readErrno);
    if(ret <= 0 && readErrno != EAGAIN){
        CloseConn_(client);
        return;
    }
    OnProcess(client);
}
void WebServer::OnWrite_(HttpConn* client){
    int ret = -1;
    int writeErrno = 0;
    ret = client->write(&writeErrno);
    if(client->ToWriteBytes() == 0){
        if(client->IsKeepAlive()){
            epoller_->ModFd(client->getFd(), connEvent_ | EPOLLOUT);
            return;
        }
    }
    else if(ret < 0){
        if(writeErrno == EAGAIN){
            epoller_->ModFd(client->getFd(), connEvent_ | EPOLLOUT);
            return;
        }
    }
    CloseConn_(client);
}
void WebServer::OnProcess(HttpConn* client){
    if(client->process()){
        epoller_->ModFd(client->getFd(), connEvent_ | EPOLLOUT);
    }else{
        epoller_->ModFd(client->getFd(), connEvent_ | EPOLLIN);
    }
}

void WebServer::Start(){
    int timeMS = -1;
    if(!isClose_) { LOG_INFO("========== Server start =========="); }
    while (!isClose_)
    {
        if(timeoutMS_>0){
            timeMS = timer_->GetNextTick();
        }
        int eventCnt = epoller_->Wait(timeMS);
        for(int i=0; i<eventCnt; i++){
            int fd = epoller_->GetEventFd(i);
            uint32_t events = epoller_->GetEvents(i);

            if(fd == listenFd_){
                DealListen_();
            }
            else if(events & (EPOLLRDHUP | EPOLLHUP | EPOLLERR)){
                CloseConn_(&users_[fd]);
            }
            else if(events & EPOLLIN){
                DealRead_(&users_[fd]);
            }
            else if(events & EPOLLOUT){
                DealWrite_(&users_[fd]);
            }
            else{
                LOG_ERROR("Unexpected event");
            }
        }
    }
    
}