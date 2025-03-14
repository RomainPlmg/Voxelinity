#ifndef __LOGGER_H__
#define __LOGGER_H__

#include <format>
#include <iostream>
#include <mutex>

class Logger {
   public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    static void Init();
    static void Shutdown();

    static Logger& Get();

    template <typename... Args>
    void Trace(const std::string_view& formatStr, Args&&... args);
    template <typename... Args>
    void Info(const std::string_view& formatStr, Args&&... args);
    template <typename... Args>
    void Warning(const std::string_view& formatStr, Args&&... args);
    template <typename... Args>
    void Error(const std::string_view& formatStr, Args&&... args);
    template <typename... Args>
    void Fatal(const std::string_view& formatStr, Args&&... args);

   private:
    Logger() = default;
    ~Logger() = default;

   private:
    std::mutex m_LoggerMutex;
};

#define ANSI_RESET "\033[0m"
#define ANSI_RED "\033[31m"
#define ANSI_GREEN "\033[32m"
#define ANSI_YELLOW "\033[33m"
#define ANSI_BLUE "\033[34m"

template <typename... Args>
void Logger::Trace(const std::string_view& formatStr, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    std::cout << ANSI_BLUE << "[TRACE] " << std::vformat(formatStr, std::make_format_args(args...)) << ANSI_RESET << "\n";
}

template <typename... Args>
void Logger::Info(const std::string_view& formatStr, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    std::cout << ANSI_GREEN << "[INFO] " << std::vformat(formatStr, std::make_format_args(args...)) << ANSI_RESET << "\n";
}

template <typename... Args>
void Logger::Warning(const std::string_view& formatStr, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    std::cout << ANSI_YELLOW << "[WARNING] " << std::vformat(formatStr, std::make_format_args(args...)) << ANSI_RESET << "\n";
}

template <typename... Args>
void Logger::Error(const std::string_view& formatStr, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    std::cout << ANSI_RED << "[ERROR] " << std::vformat(formatStr, std::make_format_args(args...)) << ANSI_RESET << "\n";
}

template <typename... Args>
void Logger::Fatal(const std::string_view& formatStr, Args&&... args) {
    std::lock_guard<std::mutex> lock(m_LoggerMutex);
    std::cout << ANSI_RED << "[FATAL] " << std::vformat(formatStr, std::make_format_args(args...)) << ANSI_RESET << "\n";
    std::abort();
}

#define LOG_TRACE(...) Logger::Get().Trace(__VA_ARGS__)
#define LOG_INFO(...) Logger::Get().Info(__VA_ARGS__)
#define LOG_WARNING(...) Logger::Get().Warning(__VA_ARGS__)
#define LOG_ERROR(...) Logger::Get().Error(__VA_ARGS__)
#define LOG_FATAL(...) Logger::Get().Fatal(__VA_ARGS__)

#endif  // __LOGGER_H__