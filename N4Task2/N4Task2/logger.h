#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static Logger& getInstance();

    void setLogFile(const std::string& filename);
    void debug(const std::string& message);
    void info(const std::string& message);
    void warning(const std::string& message);
    void error(const std::string& message);

    ~Logger();

private:
    Logger();
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    void log(const std::string& level, const std::string& message);
    std::string getCurrentTimestamp();

    std::ofstream m_logFile;
    std::mutex m_mutex;
    bool m_isInitialized;
};

#endif
