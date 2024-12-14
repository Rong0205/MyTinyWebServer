#include "conpool.h"


///////////////////////Connnection类实现///////////////////////
//初始化数据库连接
Connection::Connection(){
    m_conn = mysql_init(nullptr);
}
//释放数据库资源
Connection::~Connection(){
    if(m_conn != nullptr){
        mysql_close(m_conn);
    }
}
//连接数据库
bool Connection::connect(std::string ip, unsigned short port, std::string username, std::string passwd, std::string dbname){
    MYSQL *ptr = mysql_real_connect(m_conn, ip.c_str(), username.c_str(), passwd.c_str(), dbname.c_str(), port, nullptr, 0);
    return ptr != nullptr;
}
//更新操作
bool Connection::update(std::string sql){
    return !mysql_query(m_conn, sql.c_str());
}
//查询操作
MYSQL_RES* Connection::query(std::string sql){
    if(mysql_query(m_conn, sql.c_str())) return nullptr;
    return mysql_use_result(m_conn);
}

/////////////////////////ConnectionPool////////////////////////////
ConnectionPool* ConnectionPool::getConnectionPool(std::string ip, unsigned short port, std::string username,
                     std::string passwd, std::string dbname, unsigned int initConns,
                     unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout){
    static ConnectionPool pool(ip, port, username, passwd, dbname, initConns, maxConns, maxIdleTime, connectTimeout);
    return &pool;
}

ConnectionPool::ConnectionPool(std::string ip, unsigned short port, std::string username,
                     std::string passwd, std::string dbname, unsigned int initConns,
                     unsigned int maxConns, unsigned int maxIdleTime, unsigned int connectTimeout)
: m_ip(ip), m_port(port), m_username(username), m_passwd(passwd), m_dbname(dbname),
  m_initConns(initConns), m_maxConns(maxConns), m_maxIdleTime(maxIdleTime),
  m_connectTimeout(connectTimeout)
{
    for(int i = 0; i< m_initConns; ++i){
        Connection* conn = new Connection();
        conn->connect(m_ip, m_port, m_username, m_passwd, m_dbname);
        m_connectionQue.push(conn);
        m_connectionCnt++;
    }
}

Connection* ConnectionPool::getConnection(){

}