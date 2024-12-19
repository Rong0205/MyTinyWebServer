#ifndef HTTP_RESPONSE_H
#define HTTP_RESPONSE_H

#include <unordered_map>
#include <fcntl.h>       // open
#include <unistd.h>      // close
#include <sys/stat.h>    // stat
#include <sys/mman.h>    // mmap, munmap

#include "buffer.h"
#include "log.h"

class HttpResponse {
public:

    HttpResponse();
    ~HttpResponse();

    void Init(const std::string& srcDir, std::string& path, bool isKeepAlive = false, int code = -1); //初始化
    void MakeResponse(Buffer& buff);    //生成响应报文
    void UnmapFile();       //取消映射
    char* getFile();           //映射文件
    size_t getFileLen() const; //映射文件长度
    void ErrorContent(Buffer& buff, std::string message);   //映射文件错误响应报文
    int getCode() const { return code_; }   //获取状态码

private:
    void AddStateLine_(Buffer &buff);   //添加状态行
    void AddHeader_(Buffer &buff);      //添加响应头
    void AddContent_(Buffer &buff);     //添加响应体

    void ErrorHtml_();                  //错误界面文件
    std::string GetFileType_();         //获取文件类型

    int code_;
    bool isKeepAlive_;

    std::string path_;
    std::string srcDir_;
    
    char* mmFile_; 
    struct stat mmFileStat_;

    static const std::unordered_map<std::string, std::string> SUFFIX_TYPE;  // 后缀类型集
    static const std::unordered_map<int, std::string> CODE_STATUS;          // 编码状态集
    static const std::unordered_map<int, std::string> CODE_PATH;            // 编码路径集
};


#endif //HTTP_RESPONSE_H

