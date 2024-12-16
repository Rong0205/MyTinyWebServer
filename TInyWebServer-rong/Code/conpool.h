#ifndef CONPOOL_H
#define CONPOLL_H

#include <iostream>
#include <mysql/mysql.h>
#include <string>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <thread>
#include <memory>
#include <functional>
#include <chrono>
#include <ctime>

class Connection{
public:
    //初始化数据库连接
    Connection();
    //释放数据库资源
    ~Connection();
    //连接数据库
    bool connect(std::string ip, unsigned short port, std::string username, std::string passwd, std::string dbname);
    //更新操作
    bool update(std::string sql);
    //查询操作
    MYSQL_RES *query(std::string sql);
    //刷新当前空闲时间
    void refreshAlive(){m_alivetime = clock();}
    //返回存活的时间
    clock_t getAlive(){return clock() - m_alivetime;} 

private:
    MYSQL *m_conn;
    clock_t m_alivetime; //进入空闲状态后的存活时间

};

class ConnectionPool{
public:
    static ConnectionPool* getConnectionPool(std::string ip, unsigned short port, std::string username,
                                             std::string passwd, std::string dbname, unsigned int initConns,
                                             unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout);

    std::shared_ptr<Connection> getConnection();


private:
    ConnectionPool(std::string ip, unsigned short port, std::string username,
                   std::string passwd, std::string dbname, unsigned int initConns,
                   unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout);

    ~ConnectionPool(){
        {
            std::unique_lock<std::mutex> lock(m_queMutex);
            m_isClosed = true;
        }
        m_cv.notify_all();
        std::cout << "ConnPool join" << std::endl;
        m_produceThread.join();
        //m_scannerThread.join();
        std::cout << "ConnPool join done" << std::endl;
    }

    void produceConnTask();
    void scannerConnTask();

    std::string m_ip;
    unsigned short m_port;
    std::string m_username;
    std::string m_passwd;
    std::string m_dbname;
    unsigned int m_initConns;
    unsigned int m_maxConns;
    unsigned int m_maxIdleTime;
    unsigned int m_connectTimeout;

    std::queue<Connection*> m_connectionQue;
    std::mutex m_queMutex;
    std::atomic_int m_connectionCnt;
    std::condition_variable m_cv;
    bool m_isClosed;
    //std::atomic_int m_freeConnCnt;
    std::thread m_scannerThread;
    std::thread m_produceThread;
};

#endif