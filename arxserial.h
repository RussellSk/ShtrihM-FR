#ifndef ARXSERIAL_H
#define ARXSERIAL_H

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio.hpp>
#include <boost/asio/streambuf.hpp>
#include <boost/system/system_error.hpp>
#include <boost/asio/serial_port.hpp>
#include <boost/lambda/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/bind.hpp>
#include <boost/chrono.hpp>
#include <iostream>
#include <vector>

using boost::asio::deadline_timer;
using boost::lambda::bind;
using boost::lambda::var;

typedef boost::shared_ptr<boost::asio::serial_port> serial_port_ptr;
typedef unsigned char byte;
typedef std::vector<unsigned char> vec_bytes;

class ArxSerial
{

    boost::asio::io_service io_service;
    boost::asio::streambuf input_buffer_;
    boost::posix_time::time_duration m_timeout;
    deadline_timer deadline_;
    byte c;
    void check_deadline();
    byte _read_char();
    void _write_line(vec_bytes data);
public:
    serial_port_ptr p_port;
    ArxSerial(int timeout);
    int Connect(std::string SerialName, int baud_rate = 9600);
    void write_line(vec_bytes data);

    byte get_byte();
    vec_bytes readLine(void);
    void write_byte(byte cmd);
};

#endif // ARXSERIAL_H
