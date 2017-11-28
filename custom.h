#ifndef CUSTOM_H
#define CUSTOM_H
#include <cstdlib>
#include <iomanip>
#include <string>
#include <map>
#include <boost/thread.hpp>
#include <boost/chrono.hpp>

#include "custompacket.h"
#include "arxserial.h"
#include "customcommands.h"

// Максимальное количество попыток
#define MAX_CLEARANSWER_TRIES 10
#define MIN_TIMEOUT 0.25

const vec_bytes DEFAULT_ADMIN_PASSWORD = {0x1E, 0x00, 0x00, 0x00}; // По умолчания пароль 30
const vec_bytes DEFAULT_CASSA_PASSWORD = {0x01, 0x00, 0x00, 0x00}; // По умолчанию пароль 1

class custom
{
    vec_bytes m_Password;

    CustomPacket CPacket;
    ArxSerial *ComPort;
    std::string COMNAME;
    int COMBAUD;
    void recconect_serial();
public:
    custom(std::string COM, int baudrate, vec_bytes password = DEFAULT_ADMIN_PASSWORD);

    // Проверка состояния на готовность
    byte checkState();

    // Обработка и отправка команды
    void sendCommand(vec_bytes Data = {});

    // Чтение и преобразование пакета в удобный вид
    vec_bytes readAnswer(void);

    // Отчищает пул между хостом и сервером для следующей команды
    int clearAnswer(void);

    // Печать X отчета без гашения
    bool printXreport(void);

    // Короткий запрос статуса фискальника
    bool shortStatusRequest(void);

    // Full Status Request
    bool statusRequest(void);

    // print result
    void print_r(vec_bytes data);

    // Открыть продажу
    int Sale(int count, int price, std::string text, int department = 1);

    // Закрыть чек
    int closeReceipt(int summa, int sale, std::string text);

    // Суточный отчет с гашением
    int reportZclose(void);

    // Гудок
    int beep(void);

    // Retuns current Custom mode.
    int statusMode();

    // Анулировать чек
    void avoidCheck();

    // Подитожить чек
    void poditog();

    // Тестовая печать чека
    void testReceipt();

    void PrintString(std::string text);

};

#endif // CUSTOM_H
