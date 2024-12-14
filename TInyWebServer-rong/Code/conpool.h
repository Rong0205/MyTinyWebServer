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

private:
    MYSQL *m_conn;
};

class ConnectionPool{
public:
    static ConnectionPool* getConnectionPool(std::string ip, unsigned short port, std::string username,
                                             std::string passwd, std::string dbname, unsigned int initConns,
                                             unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout);

    Connection* getConnection();


private:
    ConnectionPool(std::string ip, unsigned short port, std::string username,
                   std::string passwd, std::string dbname, unsigned int initConns,
                   unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout);

    std::string m_ip;
    unsigned short m_port;
    std::string m_username;
    std::string m_passwd;
    std::string m_dbname;
    unsigned int m_maxConns;
    unsigned int m_initConns;
    unsigned int m_maxIdleTime;
    unsigned int m_connectTimeout;

    std::queue<Connection*> m_connectionQue;
    std::mutex m_queMutex;
    std::atomic_int m_connectionCnt;
};

#endif