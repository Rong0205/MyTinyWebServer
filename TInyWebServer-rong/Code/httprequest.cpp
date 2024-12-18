#include "httprequest.h"

const std::unordered_set<std::string> HttpRequest::DEFAULT_HTML{
            "/index", "/register", "/login",
             "/welcome", "/video", "/picture", };

const std::unordered_map<std::string, int> HttpRequest::DEFAULT_HTML_TAG {
            {"/register.html", 0}, {"/login.html", 1},  };

void HttpRequest::Init() {
    method_ = path_ = version_ = body_ = "";
    state_ = REQUEST_LINE;
    header_.clear();
    post_.clear();
}

bool HttpRequest::ParseRequestLine_(const std::string& line){
    std::regex pattern("^([^ ]*) +([^ ]*) +([^ ]*)$");
    std::smatch subMatch;

    if(std::regex_match(line, subMatch, pattern)){
        method_ = subMatch[1];
        path_ = subMatch[2];
        version_ = subMatch[3];
        state_ = HEADERS;
        return true;
    }
    LOG_ERROR("RequestLine Error!");
    return false;
}

void HttpRequest::ParseHeader_(const std::string& line){
    std::regex pattern("^([^:]*): ?(.*)$");
    std::smatch subMatch;
    if(std::regex_match(line, subMatch, pattern)){
        header_[subMatch[1]] = subMatch[2];
    }
    else{
        state_ = BODY;
    }
}
void HttpRequest::ParseBody_(const std::string& line, ConnectionPool* pool){
    body_ = line;
    ParsePost_(pool);
    state_ = FINISH;
    LOG_DEBUG("body:%s, len:%d", body_.c_str(), body_.size());
}

int HttpRequest::ConverHex(char ch){
    if(ch >= 'A' && ch <= 'F') return ch- 'A' + 10;
    if(ch >= 'a' && ch <= 'f') return ch- 'a' + 10;
    return ch;
}

bool HttpRequest::IsKeepAlive() const{
    if(header_.count("Connection")){
        return header_.find("Connection")->second == "keep-alive" && version_ == "1.1";
    }
    return false;
}

void HttpRequest::ParsePath_(){
    if(path_ == "/"){
        path_ = "/index.html";
    }
    else{
        for(auto &item : DEFAULT_HTML){
            if(item == path_){
                path_ += ".html";
                break;
            }
        }
    }
}

void HttpRequest::ParseFromUrlencoded_(){
    if(body_.size() == 0){ return; }

    std::string key, value;
    int num = 0;
    int n = body_.size();
    int i = 0, j = 0;
    for(; i<n; i++){
        char ch = body_[i];
        switch(ch){
            case '=':
                key = body_.substr(j, i-j);
                j = i+1;
                break;
            case '+':
                body_[i] = ' ';
                break;
            case '%':
                num = ConverHex(body_[i+1])*16 + ConverHex(body_[i+2]);
                body_[i+2] = num % 10 + '0';
                body_[i+1] = num / 10 + '0';
                i += 2;
                break;
            case '&':
                value = body_.substr(j, i-j);
                j = i+1;
                post_[key] = value;
                LOG_DEBUG("post[%s] = %s", key.c_str(), value.c_str());
                break;
            default:
                break;
        }
    }
    if(post_.count(key) == 0 && j<i){
        value = body_.substr(j, i-j);
        post_[key] = value;
    }
}

void HttpRequest::ParsePost_(ConnectionPool* pool){
    if(method_ == "POST" && header_["Content-Type"] == "application/x-www-from-urlencoded"){
        ParseFromUrlencoded_();
        if(DEFAULT_HTML_TAG.count(path_)){
            int tag = DEFAULT_HTML_TAG.find(path_)->second;
            LOG_DEBUG("tag:%d", tag);
            int verify_flag = false;
            if(tag == 1) verify_flag = UserVerifyLogin(post_["username"], post_["password"], pool);
            else verify_flag = UserVerifyRegister(post_["username"], post_["password"], pool);

            if(verify_flag) path_ = "/welcome.html";
            else path_ = "/error.html";
        }
    }
}

bool HttpRequest::parse(Buffer& buff, ConnectionPool* pool){
    const char CRLF[] = "\r\n";
    if(buff.ReadableBytes() <= 0) return false;

    while(buff.ReadableBytes() && state_ != FINISH){
        const char* lineEnd = std::search(buff.Peek(), buff.BeginWriteConst(), CRLF, CRLF + 2);

        std:: string line(buff.Peek(), lineEnd);
        switch(state_){
            case REQUEST_LINE:
                if(!ParseRequestLine_(line))
                {
                    return false;
                }
                ParsePath_();
                break;
            case HEADERS:
                ParseHeader_(line);
                if(buff.ReadableBytes() <= 2 ){
                    state_ = FINISH;
                }
                break;
            case BODY:
                ParseBody_(line, pool);
                break;
            default:
                break;
        }   
        if(lineEnd == buff.BeginWrite()) {break;}
        buff.RetrieveUntil(lineEnd + 2);
    }
    //日志输出method,path,version
    return true;
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
        std::string password(row[0]);
        if (pwd == password) {
            verify_flag = true;
            LOG_DEBUG("UserVerifyLogin success!!");
        } else {
            LOG_INFO("pwd error!");
            //std::cout << "pwd error!" << std::endl;
            verify_flag = false;
        }
    } else {
        LOG_INFO("user not found!");
        verify_flag = false;
    }

    mysql_free_result(result);
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
        std:: cout << "mysql_query error" << std::endl;
        return false;
    }

    // 判断用户是否已存在
    if (mysql_fetch_row(result)) {
        LOG_INFO("user used!");
        //std::cout << "user used!" << std::endl;
        mysql_free_result(result); 
        //verify_flag = false;
        return verify_flag;
    }

    mysql_free_result(result);    

    bzero(order, 256);
    snprintf(order, 256, "INSERT INTO users(username, password) VALUES('%s','%s')", name.c_str(), pwd.c_str());
    std::string orderStr1(order);

    //LOG_DEBUG("%s", order);
    if (!sqlCon->update(orderStr1)) {
        LOG_ERROR("Insert error");
        //verify_flag = false;
    }
    else{
        verify_flag = true;
        LOG_DEBUG("UserVerifyRegister success!!");
    }
    return verify_flag;
}

std::string HttpRequest::getPath() const{
    return path_;
}
std::string& HttpRequest::getPath(){
    return path_;
}
std::string HttpRequest::getMethod() const{
    return method_;
}
std::string HttpRequest::getVersion() const{
    return version_;
}
std::string HttpRequest::getPost(const std::string& key) const{
    if(key != ""){
        return post_.find(key)->second;
    }
    return "";
}
std::string HttpRequest::getPost(const char* key) const{
    if(post_.count(key) == 1){
        return post_.find(key)->second;
    }
    return "";
}

