#include "conpool.h"

int main(){
    
    clock_t start = clock();

    ConnectionPool* connpool = ConnectionPool::getConnectionPool("127.0.0.1", 3306, "root", "20010205", "test", 10, 1024, 60, 100);
    for(int i = 0; i<1000; i++){
        std::string sql = "insert into example_table(name, age) values('zhangsan', 18);";
        std::shared_ptr<Connection> conn = connpool->getConnection();
        conn->update(sql);
    }
    clock_t end = clock();

    std::cout << "cost time: " << double(end - start)/CLOCKS_PER_SEC*1000 << std::endl;
    std::cout << "hello world" << std::endl;
    _Exit(0);
}