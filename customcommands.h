#ifndef CUSTOMCOMMANDS_H
#define CUSTOMCOMMANDS_H

enum class CustomCommand : unsigned char
{
    ENQ = 0x05,
    STX = 0x02,
    ACK = 0x06,
    NAK = 0x15,
};

#endif // CUSTOMCOMMANDS_H
