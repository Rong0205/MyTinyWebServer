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

    
    char testStr[] = "GET / HTTP/1.1\r\nHost: 127.0.0.1:8080\r\nConnection: keep-alive\r\nCache-Control: max-age=0\r\nUpgrade-Insecure-Requests: 1\r\nUser-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36(KHTML, like Gecko) Chrome/80.0.398";
    buff.Append(testStr, sizeof(testStr));
    std::cout << sizeof(testStr) << std::endl;
    std::cout << buff.ReadableBytes() << std::endl;

    HttpRequest request;
    request.parse(buff, connPool);



    
    return 0;
}