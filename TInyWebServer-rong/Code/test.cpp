#include <unistd.h>
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
    // bool is = request.parse(buff);
    //if(is == true) std::cout << "parse success" << std::endl;
    //getchar();


    WebServer server(2000, 3, 600000, true,
                     3306, "root", "20010205", "test", 20, 2048, 10, 100,
                     4, true, 0, 1024);
    server.Start();    

    return 0;
}