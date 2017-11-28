#include "arxserial.h"

void ArxSerial::check_deadline()
{
    if (this->deadline_.expires_at() <= deadline_timer::traits_type::now())
    {
        boost::system::error_code ignored_ec;
        this->p_port->cancel();
        this->deadline_.expires_at(boost::posix_time::pos_infin);
    }
    this->deadline_.async_wait(bind(&ArxSerial::check_deadline, this));
}

ArxSerial::ArxSerial(int timeout)
    : deadline_(io_service), m_timeout(boost::posix_time::milliseconds(timeout))
{
    deadline_.expires_at(boost::posix_time::pos_infin);
    this->check_deadline();
    this->p_port = serial_port_ptr(new boost::asio::serial_port(this->io_service));
}

int ArxSerial::Connect(std::string SerialName, int baud_rate)
{
    boost::system::error_code ec;
    this->p_port->open(SerialName.c_str(), ec);
    if (ec) {
        std::cout << "Error: this->_port->open() failed. Com port name=" << ", Error Code=" << ec.message().c_str() << std::endl;
        return 0;
    }
    this->p_port->set_option(boost::asio::serial_port_base::baud_rate(baud_rate));
    this->p_port->set_option(boost::asio::serial_port_base::character_size(8));
    this->p_port->set_option(boost::asio::serial_port_base::stop_bits(boost::asio::serial_port_base::stop_bits::one));
    this->p_port->set_option(boost::asio::serial_port_base::parity(boost::asio::serial_port_base::parity::none));
    this->p_port->set_option(boost::asio::serial_port_base::flow_control(boost::asio::serial_port_base::flow_control::none));
    return 1;
}

void ArxSerial::write_line(vec_bytes data)
{
    try {
        _write_line(data);
    } catch(std::exception const& ex) {
        std::cout << "SERIALPORT Timeout write_line: " << ex.what() << std::endl;
    }
}

void ArxSerial::_write_line(vec_bytes data)
{
    if( ! this->p_port) {
        std::cout << "SERIALPORT closed\n";
        return ;
    }
    this->p_port->get_io_service().reset();
    this->deadline_.expires_from_now(m_timeout);
    boost::system::error_code ec = boost::asio::error::would_block;
    boost::asio::async_write(*this->p_port, boost::asio::buffer(data, data.size()), var(ec) = boost::lambda::_1);

    do this->io_service.run_one(); while (ec == boost::asio::error::would_block);

    if (ec)
        throw boost::system::system_error(ec);
}

byte ArxSerial::_read_char()
{
    if( ! this->p_port) {
        std::cout << "SERIALPORT closed\n";
        return 0x00;
    }
    byte val = c = '\n';
    this->p_port->get_io_service().reset();
    this->deadline_.expires_from_now(m_timeout);
    boost::system::error_code ec = boost::asio::error::would_block;
    boost::asio::async_read(*this->p_port, boost::asio::buffer(&c, 1), var(ec) = boost::lambda::_1);

    do this->io_service.run_one(); while (ec == boost::asio::error::would_block);

    if (ec == boost::system::errc::operation_canceled){
        return '\n';
    }

    if (ec)
        throw boost::system::system_error(ec);

    val = c;
    return val;
}

byte ArxSerial::get_byte()
{
    byte val;
    try {
        val = _read_char();
    } catch(std::exception const& ex) {
        std::cout << "SERIALPORT Timeout get_byte: " << ex.what() << std::endl;
        //return (byte)'\n';
    }
    return val;
}

vec_bytes ArxSerial::readLine()
{
    vec_bytes result;
    byte val;
    std::cout << "readLine()\n";
    while(true) {
        val = get_byte();
        switch(val)
        {
            case '\r':
                break;
            case '\n':
                return result;
            default:
                result.push_back(val);
        }
    }
}

void ArxSerial::write_byte(byte cmd)
{
    std::vector<byte> buff = { cmd };
    try {
        write_line(buff);
    } catch(std::exception const& ex) {
        std::cout << "SERIALPORT Timeout write_byte: " << ex.what() << std::endl;
    }
}
