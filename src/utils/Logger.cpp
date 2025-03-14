#include "Logger.h"

#include "pch.h"

/*static*/ Logger* s_LoggerInst = nullptr;

void Logger::Init() { s_LoggerInst = new Logger(); }

void Logger::Shutdown() {
    delete s_LoggerInst;
    s_LoggerInst = nullptr;
}

Logger& Logger::Get() {
    assert(s_LoggerInst != nullptr);
    return *s_LoggerInst;
}
