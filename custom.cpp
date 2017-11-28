#include "custom.h"

void custom::recconect_serial()
{
    this->ComPort->p_port->close();
    boost::this_thread::sleep_for(boost::chrono::seconds(1));
    this->ComPort->Connect(COMNAME, COMBAUD);
}

custom::custom(std::string COM, int baudrate, vec_bytes password)
    : m_Password(password), COMNAME(COM), COMBAUD(baudrate)
{
    ComPort = new ArxSerial(700);
}

vec_bytes custom::readAnswer()
{
    vec_bytes buffer;

    // Первый байт должен быть ACK
    byte STAT = this->ComPort->get_byte();
    if(STAT == (byte)CustomCommand::ACK)
    {
        // Второй байт должен быть равен STX
        if(this->ComPort->get_byte() == (byte)CustomCommand::STX)
        {
            byte length = this->ComPort->get_byte();
            byte cmd = this->ComPort->get_byte();
            byte errcode = this->ComPort->get_byte();

            for(int i = 0; i < length - 2; i++)
                buffer.push_back(this->ComPort->get_byte());

            // Проверка длины принятого пакета с заявленной длиной
            if(length - 2  != buffer.size()) {
                ComPort->write_byte((byte)CustomCommand::NAK);
                return buffer;
            }

            byte CRC = this->ComPort->get_byte();

            // Calculate CRC of recieved data
            vec_bytes packet = { length, cmd, errcode };
            packet.insert(std::end(packet), std::begin(buffer), std::end(buffer));
            byte RCRC = CPacket.GetCRC16(packet);

            // Проверка
            if(CRC != RCRC) {
                ComPort->write_byte((byte)CustomCommand::NAK);
                return buffer;
            }

            // Данные получены успешно
            ComPort->write_byte((byte)CustomCommand::ACK);
            boost::this_thread::sleep_for(boost::chrono::milliseconds((int)MIN_TIMEOUT));
        }
    } else if (STAT == (byte)CustomCommand::NAK) {
        return buffer;
    }

    return buffer;
}

int custom::clearAnswer()
{
    short int n = 0;
    while(MAX_CLEARANSWER_TRIES > n)
    {

        recconect_serial();

        boost::this_thread::sleep_for(boost::chrono::seconds(1));
        ComPort->write_byte((byte)CustomCommand::ENQ);
        byte a = this->ComPort->get_byte();
        if ( a == (byte)CustomCommand::NAK) {
            boost::this_thread::sleep_for(boost::chrono::milliseconds((int)MIN_TIMEOUT));
            return 1;
        } else if (a == (byte)CustomCommand::ACK) {
            a = this->ComPort->get_byte();
            boost::this_thread::sleep_for(boost::chrono::milliseconds((int)MIN_TIMEOUT));
            if (a != (byte)CustomCommand::STX) {
                return 2;
            }
            int length = this->ComPort->get_byte();
            boost::this_thread::sleep_for(boost::chrono::milliseconds((int)MIN_TIMEOUT));
            for(int i = 0; i < length+1; i++) {
                this->ComPort->get_byte();
            }
            ComPort->write_byte((byte)CustomCommand::ACK);
            boost::this_thread::sleep_for(boost::chrono::milliseconds((int)MIN_TIMEOUT));
            n++;
            continue;
        }
    }
    return 1;
}


void custom::sendCommand(vec_bytes Data)
{
   // this->print_r(Data);
    this->ComPort->write_line(Data);
}

// Печать Х отчета без гашения
bool custom::printXreport()
{
    ComPort->Connect(COMNAME, COMBAUD);

    vec_bytes result;
    this->clearAnswer();
    this->sendCommand(this->CPacket.getPack(0x40, DEFAULT_ADMIN_PASSWORD));
    result = this->readAnswer();
    this->print_r(result);

    ComPort->p_port->close();
}

// Короткий запрос статуса фискальника
bool custom::shortStatusRequest()
{
    vec_bytes result;
    this->clearAnswer();
    this->sendCommand(this->CPacket.getPack(0x10, DEFAULT_ADMIN_PASSWORD));
    result = this->readAnswer();
    this->print_r(result);
}

bool custom::statusRequest()
{
    ComPort->Connect(COMNAME, COMBAUD);

    this->clearAnswer();

    vec_bytes result;
    this->sendCommand(this->CPacket.getPack(0x11, DEFAULT_ADMIN_PASSWORD));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    result = this->readAnswer();

    // NAK Received
    if(result.size() < 3) {
        print_r(result);
        ComPort->p_port->close();
        return false;
    }

    vec_bits bits = this->CPacket.byte2bits(result[11]);
    vec_bits bits2 = this->CPacket.byte2bits(result[12]);
    bits.insert(bits.end(), bits2.begin(), bits2.end());

    std::cout << "Operator:\t" << (int)result[0]
              << "\nfr_ver:\t\t" << result[1] << '.' << result[2]
              << "\nfr_build:\t" << std::dec << (int)result[3] << (int)result[4]
              << "\nfr_date:\t" << std::dec << (int)result[5] << '/' << (int)result[6] << '/' << (int)result[7]
              << "\nzal_num:\t" << (int)result[8]
              << "\ndoc_num:\t" << (int)result[9] << (int)result[10]
              << "\nfr_flags:\t" << (int)result[11] << (int)result[12]
              << "\nmode:\t\t" << (int)result[13]
              << "\nsubmode:\t" << (int)result[14]
              << "\nfr_port:\t" << (int)result[15]
              << "\nfactory_number:\t" << std::dec << (int)result[30] << (int)result[31] << (int)result[32] << (int)result[33]
              << "\n-------------------------------------"
              << "\nrull_oper_lot:\t\t\t" << bits[0]
              << "\nrull_check_log:\t\t\t" << bits[1]
              << "\nupper_sensor_skid_document:\t" << bits[2]
              << "\nlower_sensor_skid_document:\t" << bits[3]
              << "\neklz:\t\t\t\t" << bits[5]
              << "\noptical_sensor_oper_log:\t" << bits[6]
              << "\noptical_sensor_chek_log:\t" << bits[7]
              << "\nthermo_oper_log:\t\t" << bits[8]
              << "\nthermo_check_log:\t\t" << bits[9]
              << "\nbox_open:\t\t\t" << bits[10]
              << "\nmoney_box_open:\t\t\t" << bits[11]
              << "\neklz_full:\t\t\t" << bits[14]
              << std::endl;

    ComPort->p_port->close();
    return true;
}

void custom::print_r(vec_bytes data)
{
    for(auto byte : data)
            std::cout << std::setw(2) << std::setfill('0') << std::hex << std::uppercase << (int)byte << ' ';
    std::cout << std::endl;
}

int custom::Sale(int count, int price, std::string text, int department)
{
    /*
        Команда:
        80H. Длина сообщения: 60 или 20+Y 1,2 байт.
        Пароль оператора (4 байта)
        Количество (5 байт) 0000000000...9999999999
        Цена (5 байт) 0000000000...9999999999
        Номер отдела (1 байт) 0...16 – режим свободной продажи, 255 – режим продажи
        по коду товара 1,3
        Налог 1 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Налог 2 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Налог 3 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Налог 4 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Текст 4,5,6,7 (40 или до Y 1,2 байт) строка названия товара или строка "XXXX" кода
        товара 1,3 , где XXXX = 0001...9999
        Ответ:
        80H. Длина сообщения: 3 байта.
        Код ошибки (1 байт)
        Порядковый номер оператора (1 байт) 1...30
    */
    ComPort->Connect(COMNAME, COMBAUD);

    clearAnswer();

    vec_bytes data;

    // Пароль администратора 4 байта
    data.insert(std::end(data), std::begin(DEFAULT_ADMIN_PASSWORD), std::end(DEFAULT_ADMIN_PASSWORD));

    // Количество "товаров?" 5 байт
    vec_bytes value = this->CPacket.int2hex(count*1000);
    data.insert(std::end(data), std::begin(value), std::end(value));

    // Цена 5 байт
    value = this->CPacket.int2hex(price*100);
    data.insert(std::end(data), std::begin(value), std::end(value));

    // Department number 1 байт
    data.push_back((unsigned char)department & 0xFF);

    // Taxes 4 байта
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);

    vec_bytes text_data(40, 0x00);
    for(int i = 0; i < text.length(); i++)
        text_data[i] = (byte)text[i];

    data.insert(std::end(data), std::begin(text_data), std::end(text_data));

    std::cout << "TEXT: " << text << std::endl;

    this->sendCommand(this->CPacket.getPack(0x80, data));
    print_r(this->CPacket.getPack(0x80, data));
    std::cout << "-> ";
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    print_r(this->readAnswer());

    ComPort->p_port->close();
}

int custom::closeReceipt(int summa, int sale, std::string text)
{
    /*
        Команда:
        85H. Длина сообщения: 71 или 40+Y 1,2 байт.
        Пароль оператора (4 байта)
        Сумма наличных (5 байт) 0000000000...9999999999
        Сумма типа оплаты 2 (5 байт) 0000000000...9999999999
        Сумма типа оплаты 3 (5 байт) 0000000000...9999999999
        Сумма типа оплаты 4 (5 байт) 0000000000...9999999999
        Скидка/Надбавка(в случае отрицательного значения) в % на чек от 0 до 99,99 % (2
        байта со знаком) -9999...9999
        Налог 1 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Налог 2 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Налог 3 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Налог 4 (1 байт) «0» – нет, «1»...«4» – налоговая группа
        Текст 3,4,5,6 (40 или до Y 1,2 байт)
        Ответ:
        85H. Длина сообщения: 8 байт.
        Код ошибки (1 байт)
        Порядковый номер оператора (1 байт) 1...30
        Сдача (5 байт) 0000000000...9999999999
    */

    ComPort->Connect(COMNAME, COMBAUD);

    clearAnswer();
    vec_bytes data;
    vec_bytes value;

    // Пароль администратора 4 байта
    data.insert(std::end(data), std::begin(DEFAULT_ADMIN_PASSWORD), std::end(DEFAULT_ADMIN_PASSWORD));

    // Сумма наличных 5 байт
    value = this->CPacket.int2hex(summa*100);
    data.insert(std::end(data), std::begin(value), std::end(value));

    // Сумма типа оплаты 2 (5 байт)
    value = this->CPacket.int2hex(0);
    data.insert(std::end(data), std::begin(value), std::end(value));

    // Сумма типа оплаты 3 (5 байт)
    value = this->CPacket.int2hex(0);
    data.insert(std::end(data), std::begin(value), std::end(value));

    // Сумма типа оплаты 4 (5 байт)
    value = this->CPacket.int2hex(0);
    data.insert(std::end(data), std::begin(value), std::end(value));

    // Скидка/Надбавка 2 байта
    data.push_back(0x00);
    data.push_back(0x00);

    // Taxes 4 байта
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);
    data.push_back(0x00);

    vec_bytes text_data(40, 0x00);
    for(int i = 0; i < text.length(); i++)
        text_data[i] = (byte)text[i];
    
    data.insert(std::end(data), std::begin(text_data), std::end(text_data));

    this->ComPort->write_line(this->CPacket.getPack(0x85, data));
    print_r(this->CPacket.getPack(0x85, data));
    boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    print_r(this->readAnswer());

    ComPort->p_port->close();
}

int custom::reportZclose()
{
    ComPort->Connect(COMNAME, COMBAUD);
    vec_bytes result;
    clearAnswer();
    std::cout << "Command 0x41\n";
    this->sendCommand(this->CPacket.getPack(0x41, DEFAULT_ADMIN_PASSWORD));
    boost::this_thread::sleep_for(boost::chrono::microseconds(100));
    result = this->readAnswer();
    ComPort->p_port->close();
    return 0;
}

int custom::beep()
{
    readAnswer();
    this->sendCommand(this->CPacket.getPack(0x13, DEFAULT_ADMIN_PASSWORD));
    return 0;
}

int custom::statusMode()
{
    ComPort->Connect(COMNAME, COMBAUD);
    this->clearAnswer();

    vec_bytes result;
    this->sendCommand(this->CPacket.getPack(0x11, DEFAULT_ADMIN_PASSWORD));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    result = this->readAnswer();

    // NAK Received
    if(result.size() < 3) {
        print_r(result);
        ComPort->p_port->close();
        return -1;
    }

    ComPort->p_port->close();
    return (int)result[13];
}

void custom::avoidCheck()
{
    ComPort->Connect(COMNAME, COMBAUD);
    this->clearAnswer();
    vec_bytes result;
    this->sendCommand(CPacket.getPack(0x88, DEFAULT_ADMIN_PASSWORD));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    result = this->readAnswer();
    ComPort->p_port->close();
}

void custom::poditog()
{
    ComPort->Connect(COMNAME, COMBAUD);
    this->clearAnswer();
    vec_bytes result;
    this->sendCommand(CPacket.getPack(0x89, DEFAULT_ADMIN_PASSWORD));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    result = this->readAnswer();
    ComPort->p_port->close();
}

void custom::testReceipt()
{
    ComPort->Connect(COMNAME, COMBAUD);
    clearAnswer();
    sendCommand(CPacket.getPack(0x10, DEFAULT_ADMIN_PASSWORD));
    boost::this_thread::sleep_for(boost::chrono::milliseconds(100));
    readAnswer();
    ComPort->p_port->close();
}

void custom::PrintString(std::string text)
{
    ComPort->Connect(COMNAME, COMBAUD);
    clearAnswer();
    vec_bytes data;
    byte flag = 0x00;
    flag = flag | 1;
    flag = flag | 2;
    data.insert(data.begin(), DEFAULT_ADMIN_PASSWORD.begin(), DEFAULT_ADMIN_PASSWORD.end());
    data.push_back(flag);
    vec_bytes text_data(40, 0x00);
    for(int i = 0; i < text.length(); i++)
        text_data[i] = (byte)text[i];
    data.insert(std::end(data), std::begin(text_data), std::end(text_data));
    sendCommand(CPacket.getPack(0x17, data));
    readAnswer();
    ComPort->p_port->close();
}
