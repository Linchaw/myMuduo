#ifndef BUFFER_H
#define BUFFER_H

#include<string>

enum buffer_type {fixed_buf, head_buf, http_buf};

class Buffer
{
private:
    std::string m_buf;
    buffer_type m_type;  
     
public:
    Buffer(buffer_type type = head_buf);
    ~Buffer();
    void append(const char* data, size_t size);
    void appendx(const char* data, size_t size);
    size_t size();
    const char* data();
    bool empty();
    void clear();
    void erase(size_t pos, size_t size);
    bool getdata(std::string& str);
    bool getdatax(std::string& str);
};
#endif