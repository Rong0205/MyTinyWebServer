#include "conpool.h"
#include "threadpool.h"
#include "httprequest.h"

// void testDatabaseQuery(ConnectionPool* connpool) {
//     for(int i = 0; i<1000; i++){
//         std::string sql = "insert into example_table(name, age) values('zhangsan', 18);";
//         std::shared_ptr<Connection> conn = connpool->getConnection();
//         conn->update(sql);
//     }
//     std::cout << "insert success" << std::endl;
// }
int main(){

    ConnectionPool* connPool = ConnectionPool::getConnectionPool("127.0.0.1", 3306, "root", "20010205", "test", 10, 2048, 10, 100);
    auto conn = connPool->getConnection();
    if(HttpRequest::UserVerifyLogin("rongziran", "123245", connPool)) 
    std::cout << "login success" << std::endl;
    //conn->update("insert into users(username, password) values('lisi', '12345');");
    // ConnectionPool* connPool = ConnectionPool::getConnectionPool("127.0.0.1", 3306, "root", "20010205", "test", 10, 2048, 10, 100);
    // ThreadPool* thPool = ThreadPool::getThreadPool(5);
    
    
    // for(int i = 0; i<10; i++){
    //     thPool->AddTask(testDatabaseQuery, connPool);
    // }

    // getchar();
    //thPool->~ThreadPool();
    //~ConnectionPool();

    //线程池测试
    // std::cout<<"hello world"<<std::endl;
    // ThreadPool* pool = ThreadPool::getInstance(4);
    // for(int i = 0; i< 10; i++){
    //     pool->AddTask([](){std::cout<<"hello world"<<std::endl;});
    // }

    //连接池单独测试 
    // clock_t start = clock();

    // ConnectionPool* connpool = ConnectionPool::getConnectionPool("127.0.0.1", 3306, "root", "20010205", "test", 10, 1024, 10, 10);
    // for(int i = 0; i<1000; i++){
    //     std::string sql = "insert into example_table(name, age) values('zhangsan', 18);";
    //     std::shared_ptr<Connection> conn = connpool->getConnection();
    //     conn->update(sql);
    // }
    // clock_t end = clock();

    // std::cout << "cost time: " << double(end - start)/CLOCKS_PER_SEC*1000 << std::endl;
    // std::cout << "hello world" << std::endl;
    return 0;
}