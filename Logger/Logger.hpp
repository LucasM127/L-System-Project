#ifndef LOGGER_HPP
#define LOGGER_HPP

//i got this code from somewhere i forget where then i simplified it...
//or ostream<< (ostream<<&, type T) thingy... sounds like another job day

#include <ostream>
#include <mutex>

namespace Logging
{

class Logger
{
public:
    Logger(std::ostream &ostream) : m_ostream(ostream){}
    template <typename... Args>
    void log(Args... args);
    template <typename... Args>
    void logn(Args... args);
private:
    std::ostream &m_ostream; //reference to ostream to use, not the one ...

    template <typename First, typename... Rest>
    void print_impl(First parm1, Rest... parm);
    void print_impl() {}
    std::mutex writeMutex;
};

template <typename... Args>
void Logger::log(Args... args)
{
    //lock write_mutex
    writeMutex.lock();
    print_impl(args...);
//    m_ostream << std::endl;//flushes
    m_ostream.flush();
    writeMutex.unlock();
}

template <typename... Args>
void Logger::logn(Args... args)
{
    log(args..., "\n");
}

template <typename First, typename... Rest>
void Logger::print_impl(First parm1, Rest... parm)
{
    m_ostream << parm1;
    print_impl(parm...);
}

}//namespace Logging

#ifdef LOG_DEBUG_STDOUT
#include <iostream>
static Logging::Logger logger(std::cout);
#define LOG logger.logn
#define LOG_S logger.log
#elif defined LOG_DEBUG_FILE
#include <fstream>
static std::ofstream file("log.log");
static Logging::Logger logger(file);
#define LOG logger.logn
#define LOG_S logger.log
#else
#define LOG(...)
#define LOG_S(...)
#endif //defines

#endif //LOGGER_HPP
