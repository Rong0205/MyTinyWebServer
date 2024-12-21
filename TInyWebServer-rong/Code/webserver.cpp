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
    threadpool_ = ThreadPool::getThreadPool(4);
    //connPool_ = ConnectionPool::getConnectionPool("127.0.0.1", sqlPort, sqlUser, sqlPwd, dbName, initConns, maxConns,maxIdleTime, connectTimeout);
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

bool WebServer::InitSocket_(){
    return true;
} 