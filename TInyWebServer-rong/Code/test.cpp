#include "conpool.h"
#include "threadpool.h"
#include "httprequest.h"
#include "httpresponse.h"
#include "httpconn.h"
#include "epoller.h"
#include "webserver.h"

int main(){

    // Buffer buff(1024);
    // auto connPool = ConnectionPool::getConnectionPool();
    // connPool->init("127.0.0.1", 3306, "root", "20010205", "test", 10, 2048, 10, 100);
    // //auto threadPool = ThreadPool::getThreadPool(4);
    // auto conn = connPool->getConnection();
    // conn->update("insert into users(username, password) values('rong', '123456')");
    // //int cnt = 0; 
    // int level = 0;
    // Log::Instance()->init(level, "../testlog", ".log", 0);

    // char testStr[] = 
    // "POST /login.html HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nusername=lisi&password=12345";
    // buff.Append(testStr, sizeof(testStr));
    // std::cout << sizeof(testStr) << std::endl;
    // std::cout << buff.ReadableBytes() << std::endl;

    // HttpRequest request;
    // request.parse(buff);
    WebServer server(1000, 1, 10, true,
                     3306, "root", "20010205", "test", 10, 2048, 10, 100,
                     4, true, 1, 1);


    

    return 0;
}