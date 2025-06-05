// logger.cpp
#include "logger.h"
#include <iostream> // ��� std::cerr, ���� ���-���� �� �����������
#include <iomanip>  // ��� std::put_time, std::setfill, std::setw
#include <chrono>   // ��� �������
#include <sstream>

Logger& Logger::getInstance() {
    static Logger instance;
    return instance;
}

Logger::Logger() : m_isInitialized(false) {}

Logger::~Logger() {
    if (m_logFile.is_open()) {
        info("Logger stopped.");
        m_logFile.close();
    }
}

void Logger::setLogFile(const std::string& filename) {
    std::lock_guard<std::mutex> lock(m_mutex);
    if (m_logFile.is_open()) {
        m_logFile.close();
    }
    m_logFile.open(filename, std::ios::app);
    if (!m_logFile.is_open()) {
        throw std::runtime_error("Cannot open log file: " + filename);
    }
    m_isInitialized = true;
}

void Logger::debug(const std::string& message) {
    log("DEBUG", message);
}

void Logger::info(const std::string& message) {
    log("INFO", message);
}

void Logger::warning(const std::string& message) {
    log("WARNING", message);
}

void Logger::error(const std::string& message) {
    log("ERROR", message);
}

void Logger::log(const std::string& level, const std::string& message) {
    if (!m_isInitialized) {
        throw std::runtime_error("Logger not initialized. Call setLogFile first.");
    }

    std::lock_guard<std::mutex> lock(m_mutex);
    m_logFile << "[" << getCurrentTimestamp() << "] [" << level << "] " << message << std::endl;
    m_logFile.flush();
}

std::string Logger::getCurrentTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto now_c = std::chrono::system_clock::to_time_t(now);
    auto now_ms = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()) % 1000;
    
    std::tm timeinfo;
    localtime_s(&timeinfo, &now_c);
    
    std::stringstream ss;
    ss << std::put_time(&timeinfo, "%Y-%m-%d %H:%M:%S");
    ss << "." << std::setfill('0') << std::setw(3) << now_ms.count();
    return ss.str();
}
