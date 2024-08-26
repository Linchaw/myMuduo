#ifndef TIMESTAMP_H
#define TIMESTAMP_H


#include <iostream>
#include <string>

class TimeStamp
{
public:
    TimeStamp();
    explicit TimeStamp(time_t microSencondsSinceEpoch);

    static TimeStamp now();
    std::string toString() const;
    time_t toint() const;
private:
    time_t m_microSencondsSinceEpoch;
};


#endif