#include "conpool.h"
#include "threadpool.h"

void testDatabaseQuery(ConnectionPool* connpool) {
    for(int i = 0; i<2000; i++){
        std::string sql = "insert into example_table(name, age) values('zhangsan', 18);";
        std::shared_ptr<Connection> conn = connpool->getConnection();
        conn->update(sql);
    }
    std::cout << "insert success" << std::endl;
}
int main(){

    ThreadPool* thPool = ThreadPool::getInstance(4);
    ConnectionPool* connPool = ConnectionPool::getConnectionPool("127.0.0.1", 3306, "root", "20010205", "test", 10, 1024, 10, 100);
    for(int i = 0; i<10; i++){
        thPool->AddTask(testDatabaseQuery, connPool);
    }

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