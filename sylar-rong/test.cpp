#include <iostream>
#include "log.h"

int main(){
    std::string str = "test";
    std::cout << "hello world!" << std::endl;
    sylar::Logger::ptr logger(new sylar::Logger("Rong"));
    logger->addAppender(sylar::LogAppender::ptr(new sylar::StdoutLogAppender));

    sylar::LogEvent::ptr event (new sylar::LogEvent(logger, sylar::LogLevel::DEBUG, __FILE__,__LINE__, 0, 1, 2, static_cast<uint64_t>(time(0)),str));

    logger->log(sylar::LogLevel::DEBUG, event);

    return 0;
}