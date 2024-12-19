#include "conpool.h"
#include "threadpool.h"
#include "httprequest.h"

int main(){

    Buffer buff(1024);
    ConnectionPool* connPool = ConnectionPool::getConnectionPool("127.0.0.1", 3306, "root", "20010205", "test", 10, 2048, 10, 100);
    auto conn = connPool->getConnection();
    //int cnt = 0; 
    int level = 0;
    Log::Instance()->init(level, "./testlog", ".log", 0);

    
    char testStr[] = 
    "POST /login.html HTTP/1.1\r\nHost: example.com\r\nContent-Type: application/x-www-form-urlencoded\r\n\r\nusername=lisi&password=12345";
    buff.Append(testStr, sizeof(testStr));
    std::cout << sizeof(testStr) << std::endl;
    std::cout << buff.ReadableBytes() << std::endl;

    HttpRequest request;
    request.parse(buff, connPool);




    
    return 0;
}