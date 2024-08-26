#include "TimeStamp.h"

#include<ctime>

TimeStamp::TimeStamp():m_microSencondsSinceEpoch(0) {}
TimeStamp::TimeStamp(time_t microSencondsSinceEpoch):m_microSencondsSinceEpoch(microSencondsSinceEpoch){} 

TimeStamp TimeStamp::now() {
    return TimeStamp(time(NULL));
}

std::string TimeStamp::toString() const{
    char buf[80] = {0};
    struct tm *x = localtime(&m_microSencondsSinceEpoch);
    strftime(buf, 80, "%Y-%m-%d %H:%M:%S", x);
    return buf;
}

std::time_t TimeStamp::toint() const{
    return m_microSencondsSinceEpoch;
}
