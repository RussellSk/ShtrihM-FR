#ifndef CUSTOMPACKET_H
#define CUSTOMPACKET_H

#include <iostream>
#include <vector>

typedef std::vector<unsigned char> vec_bytes;
typedef std::vector<bool> vec_bits;
typedef unsigned char byte;

class CustomPacket
{
    byte m_CMD;
    vec_bytes m_DATA;
    const byte m_STX = 0x02;
public:
    CustomPacket();

    // Установка команды
    void set_cmd(byte cmd);

    // Установка данных
    void set_data(vec_bytes data);

    vec_bytes getPack(byte cmd, vec_bytes params = {});

    // Вычисление CRC16
    byte GetCRC16(vec_bytes bufData);

    // Конвертирует байт в массив битов
    vec_bits byte2bits(byte b);

    // Конвертирует INT 2 HEX (byte)
    vec_bytes int2hex(int value);

};

#endif // CUSTOMPACKET_H
