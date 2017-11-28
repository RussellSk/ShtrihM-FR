#include "settings.h"

settings::settings() : print_test(false)
{

}

int settings::initialize()
{
    try
    {
        boost::property_tree::ptree pt;
        boost::property_tree::ini_parser::read_ini("/etc/settings.ini", pt);

        comport_name_shtrih = pt.get<std::string>("SHTRIH.comname");
        comport_baudrate_shtrih = pt.get<int>("SHTRIH.combaud");

        return 0;
    }
    catch(boost::exception &ex)
    {
        std::cout << "ERROR while reading file settings\n";
        return 1;
    }
}
