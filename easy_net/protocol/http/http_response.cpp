#include "http_response.h"

void http_response::append_to_buffer(buffer* buf)
{
    //http响应结构
    //
    //[http版本][空格][返回码][空格][状态描述信息]\r\n
    //请求头\r\n
    //\r\n
    //请求体
    buf->append(version_.c_str(), version_.size());
    buf->append(" ", 1);
    buf->append(status_code_.c_str(), status_code_.size());
    buf->append(" ", 1);
    buf->append(status_code_msg_.c_str(), status_code_msg_.size());
    buf->append("\r\n", 2);
    for (const auto& it : headers_) {
        std::string str;
        str = it.first + ":" + it.second + "\r\n";
        buf->append(str.c_str(), str.size());
    }
    buf->append("\r\n", 2);
    buf->append(body_.c_str(), body_.size());
}