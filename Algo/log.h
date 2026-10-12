#ifndef LOG_H
#define LOG_H

#include <iostream>
#include <string>

class LogHelper {
public:
    LogHelper(const char* funcName) : functionName(funcName), withMessage(false) {
        std::cerr << "Entering function: " << functionName << std::endl;
    }

    LogHelper(const char* funcName, const std::string& message) : functionName(funcName), withMessage(true) {
        std::cerr << "_____At " << message << "_____" << std::endl;
    }

    ~LogHelper() {
        if (!withMessage) {
            std::cerr << "Exiting function: " << functionName << std::endl;
        }
    }

private:
    const char* functionName;
    bool withMessage;
};

#define LOG_CONCAT_INTERNAL(a, b) a##b
#define LOG_CONCAT(a, b) LOG_CONCAT_INTERNAL(a, b)
#define lg(...) LogHelper LOG_CONCAT(logHelper_, __COUNTER__)(__FUNCTION__ __VA_OPT__(,) __VA_ARGS__)

#endif // LOG_H