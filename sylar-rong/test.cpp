#include <iostream>
#include "log.h"

int main(){
    std::string str = "test";
    std::cout << "hello world!" << std::endl;
    sylar::Logger::ptr logger(new sylar::Logger("Rong"));
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::FileLogAppender::ptr fileappender(new sylar::FileLogAppender("./log.txt"));
    sylar::LogFormatter::ptr fmt (new sylar::LogFormatter("%d%T%p%T%m%n"));
    fileappender->setFormatter(fmt);
    fileappender->setLevel(sylar::LogLevel::ERROR);
    logger->addAppender(fileappender);

    sylar::LogEvent::ptr event (new sylar::LogEvent(logger, sylar::LogLevel::DEBUG, __FILE__,__LINE__, 0, 1, 2,time(0),"str"));

    logger->log(sylar::LogLevel::DEBUG, event);
    SYLAR_LOG_INFO(logger) << "test macro";
    SYLAR_LOG_ERROR(logger) << "test macro error";

    return 0;
}