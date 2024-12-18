#include "conpool.h"
#include "threadpool.h"
#include "httprequest.h"

int main(){

    Buffer buff(1024);
    ConnectionPool* connPool = ConnectionPool::getConnectionPool("127.0.0.1", 3306, "root", "20010205", "test", 10, 2048, 10, 100);
    auto conn = connPool->getConnection();
    int cnt = 0; 
    int level = 0;
    Log::Instance()->init(level, "./testlog", ".log", 0);
    LOG_BASE(cnt++,"aaaa");
    bool istrue = HttpRequest::UserVerifyLogin("rongziran4", "12345", connPool);
    if(istrue) std::cout <<"yes"<<std::endl;
    else std::cout <<"no"<<std::endl;
    //getchar();
    
    return 0;
}