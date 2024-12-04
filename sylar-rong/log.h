#ifndef __LOG_H__
#define __LOG_H__

#include <string>
#include <memory>
#include <list>
#include <stdint.h>
#include <sstream>
#include <fstream>
#include <vector>
#include <stdarg.h>
#include <map>
//#include "util.h"
//#include "singleton.h"
//#include "thread.h"

namespace sylar{

//class Logger;
//class LoggerManager;

class LogLevel{
public:
    enum Level{
        //
        UNKOWN = 0,
        //
        DEBUG = 1,
        //
        INFO = 2,
        //
        WARN = 3,
        //
        ERROR = 4,
        //
        FATAL = 5
    };
    static const char* ToString();
    static LogLevel::Level FromString();
};

//生成日志事件
class LogEvent{
public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent();
    
    const char* getFile() const {return m_file;} 
    int32_t getLine() const { return m_line; }
    uint32_t getElapse() const { return m_elapse; }
    uint32_t getThreadId() const { return m_threadId; }
    uint32_t getFiberId() const { return m_fiberId; }
    uint64_t getTime() const { return m_time; }
    const std::string getContent() const { return m_content; }
private:
    const char* m_file = nullptr;   //文件名
    int32_t m_line = 0;             //行号
    uint32_t m_elapse = 0;          //程序启动到现在的毫秒数    
    uint32_t m_threadId = 0;        //线程号
    uint32_t m_fiberId = 0;         //携程号
    uint64_t m_time;                //时间戳
    std::string m_content;          
};

//日志器
class Logger{
public:
    using ptr = std::shared_ptr<Logger>; 
    Logger(const std::string& name = "root");
    void log(LogLevel::Level level, LogEvent::ptr event);

    void debug(LogEvent::ptr event);
    void info(LogEvent::ptr event);
    void warn(LogEvent::ptr event);
    void error(LogEvent::ptr event);
    void fatal(LogEvent::ptr event);

    void addAppender(LogAppender::ptr appender);
    void delAppender(LogAppender::ptr appender);
    LogLevel::Level getLevel() const {return m_level;};
    void setLevel(LogLevel::Level level) {m_level = level;};
private:
    std::string m_name;                     //日志名称
    LogLevel::Level m_level;                //日志级别
    std::list<LogAppender::ptr> m_appenders;//输出到目的地集合
};

//日志输出格式
class LogFormatter{
public:
    using ptr = std::shared_ptr<LogFormatter>;
    LogFormatter(const std::string& pattern);

    std::string format(LogEvent::ptr event);
public:
    class FormatItem{
    public:
        using ptr = std::shared_ptr<FormatItem>;
        virtual ~FormatItem(){};
        virtual void format(std::ostream& os, LogEvent::ptr event) = 0;
    };

    void init();
private:
    std::string m_pattern;
    std::vector<FormatItem::ptr> m_items;
    bool m_error = false;
};

//日志输出地
class LogAppender{
public:
    using ptr = std::shared_ptr<LogAppender>;
    virtual ~LogAppender(){};

    virtual void log(LogLevel::Level level, LogEvent::ptr event) = 0;

    void setformatter(LogFormatter::ptr formatter) {m_formatter = formatter;};
    LogFormatter::ptr getFormatter() const {return m_formatter;};
protected:
    LogLevel::Level m_level;
    LogFormatter::ptr m_formatter;
};

class StdoutLogAppender : public LogAppender{
public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    void log(LogLevel::Level level, LogEvent::ptr event) override;
private:
};

class FileLogAppender : public LogAppender{
public:
    using ptr = std::shared_ptr<FileLogAppender>;
    FileLogAppender(const std::string& filename);
    void log(LogLevel::Level level, LogEvent::ptr event) override;

    bool reopend();
private:
    std::string m_filename;
    std::ofstream m_filestream;
};

}

#endif


