#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <iostream>

class settings
{
public:
    int initialize();
    // Настройки режима online
    bool print_test;

    // Настройка Serial Port
    std::string comport_name_cashcode;
    std::string comport_name_shtrih;
    int comport_baudrate_cashcode;
    int comport_baudrate_shtrih;

    // Настройка идентификации
    std::string pos_id;
    std::string pos_skey;

    // Настройка сети
    std::string server_address;
    std::string server_port;

    settings();
};

#endif // SETTINGS_H
