#include "custompacket.h"

CustomPacket::CustomPacket()
{

}

void CustomPacket::set_cmd(byte cmd)
{
    m_CMD = cmd;
}

void CustomPacket::set_data(vec_bytes data)
{
    m_DATA = data;
}


// Формирование пакета для отправки
vec_bytes CustomPacket::getPack(byte cmd, vec_bytes params)
{
    vec_bytes data;
    data.push_back(cmd);
    if(params.size() > 0)
        data.insert(std::end(data), std::begin(params), std::end(params));
    int length = data.size();
    data.insert(data.begin(), (unsigned char)length & 0xFF);
    byte CRC = this->GetCRC16(data);
    data.insert(data.begin(), m_STX);
    data.push_back(CRC);
    return data;
}

byte CustomPacket::GetCRC16(vec_bytes bufData)
{
    int result = 0;
    for (auto item : bufData)
        result = result ^ item;

    return (byte)result & 0xFF;
}

vec_bits CustomPacket::byte2bits(byte b)
{
    vec_bits result;
    for(int i = 0; i < 8; i++) {
        if(b == (b >> 1 << 1))
            result.push_back(false);
        else
            result.push_back(true);
        b = b >> 1;
    }
    return result;
}

vec_bytes CustomPacket::int2hex(int value)
{
    vec_bytes result(5);
    result[3] = (value >> 24) & 0xFF;
    result[2] = (value >> 16) & 0xFF;
    result[1] = (value >> 8) & 0xFF;
    result[0] = (value & 0xFF);
    result[4] = 0x00;

    return result;
}
