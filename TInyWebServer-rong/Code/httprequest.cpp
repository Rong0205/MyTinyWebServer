#include "httprequest.h"

const std::unordered_set<string> HttpRequest::DEFAULT_HTML{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

const std::unordered_map<string, int> HttpRequest::DEFAULT_HTML_TAG {
            {"/register.html", 0}, {"/login.html", 1},  };

void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}


bool HttpRequest::UserVerifyLogin(const std::string& name, const std::string& pwd, ConnectionPool* pool){
    if (name == "" || pwd == "") return false;

    std::shared_ptr<Connection> sqlCon = pool->getConnection();

    bool verify_flag = false;

    char order[256] = { 0 };

    MYSQL_RES *result = nullptr;
    //MYSQL_FIELD *field = nullptr;
    
    snprintf(order, 256, "SELECT password FROM users WHERE username='%s' LIMIT 1", name.c_str());
    //LOG_DEBUG("%s", order);
    std::string orderStr(order);

    result = sqlCon->query(orderStr);
    if(result == nullptr) return false;

    if (MYSQL_ROW row = mysql_fetch_row(result)) {
        string password(row[0]);
        if (pwd == password) {
            verify_flag = true;
        } else {
            //LOG_INFO("pwd error!");
            std::cout << "pwd error!" << std::endl;
            verify_flag = false;
        }
    } else {
        //LOG_INFO("user not found!");
        verify_flag = false;
    }

    mysql_free_result(result);
    //LOG_DEBUG("UserVerifyLogin success!!");
    return verify_flag;    

}

bool HttpRequest::UserVerifyRegister(const std::string& name, const std::string& pwd, ConnectionPool* pool){
    if (name == "" || pwd == "") return false;

    std::shared_ptr<Connection> sqlCon = pool->getConnection();

    bool verify_flag = false;

    char order[256] = { 0 };

    MYSQL_RES *result = nullptr;
    //MYSQL_FIELD *field = nullptr;
    
    snprintf(order, 256, "SELECT password FROM users WHERE username='%s' LIMIT 1;", name.c_str());
    //LOG_DEBUG("%s", order);
    std::string orderStr(order);

    result = sqlCon->query(orderStr);
    if(result == nullptr) 
    {
        std:: cout << "mysql_query error" << endl;
        return false;
    }

    // 判断用户是否已存在
    if (mysql_fetch_row(result)) {
        //LOG_INFO("user used!");
        std::cout << "user used!" << std::endl;
        mysql_free_result(result); 
        verify_flag = false;
        return verify_flag;
    }

    mysql_free_result(result);    

    bzero(order, 256);
    snprintf(order, 256, "INSERT INTO users(username, password) VALUES('%s','%s')", name.c_str(), pwd.c_str());
    std::string orderStr1(order);

    //LOG_DEBUG("%s", order);
    if (!sqlCon->update(orderStr1)) {
        //LOG_ERROR("Insert error");
        verify_flag = false;
    }
    //LOG_DEBUG("UserVerifyLogin success!!");
    verify_flag = true;
    return verify_flag;
}

