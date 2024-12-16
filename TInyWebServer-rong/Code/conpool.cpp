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
  m_connectTimeout(connectTimeout), m_isClosed(false)
{
    for(unsigned int i = 0; i< m_initConns; ++i){
        Connection* conn = new Connection();
        conn->connect(m_ip, m_port, m_username, m_passwd, m_dbname);
        conn->refreshAlive();//刷新空闲时间
        m_connectionQue.push(conn);
        m_connectionCnt++;
    }

    //启动一个线程，作为连接的生产者
    m_produceThread = std::thread(std::bind(&ConnectionPool::produceConnTask, this));
    //produce.detach();
    //启动一个定时线程，扫描超过maxIdleTime的连接，进行释放
    //m_scannerThread = std::thread(std::bind(&ConnectionPool::scannerConnTask, this));
    //scanner.detach();
}


void ConnectionPool::produceConnTask(){
    while(true){
        std::unique_lock<std::mutex> lock(m_queMutex);
        // while (!m_connectionQue.empty())
        // {
        //     m_cv.wait(lock);
        // }
        m_cv.wait(lock, [this]{return m_connectionQue.empty() || m_isClosed;});
        if(m_isClosed) return;
        if(static_cast<unsigned int>(m_connectionCnt) < m_maxConns){
            Connection* conn = new Connection();
            conn->connect(m_ip, m_port, m_username, m_passwd, m_dbname);
            conn->refreshAlive();//刷新空闲时间
            m_connectionQue.push(conn);
            m_connectionCnt++;
        }
        m_cv.notify_one();
    }
}

void ConnectionPool::scannerConnTask(){
    while(true){
        if(m_isClosed) return;
        std::this_thread::sleep_for(std::chrono::seconds(m_maxIdleTime));
        std::unique_lock<std::mutex> lock(m_queMutex);
        while(static_cast<unsigned int>(m_connectionCnt) > m_initConns){
            Connection* conn = m_connectionQue.front();
            if(conn->getAlive() >= m_maxIdleTime*1000){
                m_connectionQue.pop();
                delete conn;
                m_connectionCnt--;
            }
            else break;
        }
    }
}
std::shared_ptr<Connection> ConnectionPool::getConnection(){
    std::unique_lock<std::mutex> lock(m_queMutex);
    while(m_connectionQue.empty()){
        if(std::cv_status::timeout == m_cv.wait_for(lock, std::chrono::milliseconds(m_connectTimeout))){
        if(m_connectionQue.empty()) return nullptr;
        //获取连接失败
        }
    }

    std::shared_ptr<Connection> sp(m_connectionQue.front(),
        [&](Connection* connPtr){
            std::unique_lock<std::mutex> lock(m_queMutex);
            connPtr->refreshAlive();
            m_connectionQue.push(connPtr);
        });
    m_connectionQue.pop();
    if(m_connectionQue.empty()){
        m_cv.notify_all();
    }
    return sp;
}