#include <iostream>
#include <chrono>
#include <thread>

#include "arxserial.h"
#include "custom.h"
#include "settings.h"

using namespace std;

static void show_usage(std::string name)
{
    std::cerr << "Usage: " << name << " <option>\n"
              << "Options:\n"
              << "\t-z, --z-report\t\tPrint Z-Report\n"
              << "\t-x, --x-report\t\tPrint X-Report\n"
              << "\t-s, --status\t\tShow full status\n"
              << "\t-m, --mode\t\tShow current Custom operation mode\n"
              << "\t-t, --text\t\tShow current Custom mode in human readable string.\n"
              << "\t-n, --null-check\tTo void a check.\n"
              << "\t-c, --close-check [price]\tClose receipt.\n"
              << "\t-o, --open-check [price]\tOpen receipt.\n"
              << "\t-p, --poditog\t\tPoditog cheka.\n"
              << "\t-h, --help\t\tShow help message.\n"
              << "\t-l  \t\tTest print mode.\n"
              << "\t-ps \t\tPrint String."
              << endl;
}

void show_human_mode(int mode)
{
    vector<string> FP_MODES_DESCR(16);
    FP_MODES_DESCR[0] = "Printer v rabochem rejime.";
    FP_MODES_DESCR[1] = "Vidacha dannih.";
    FP_MODES_DESCR[2] = "Otkritaya smena, 24 chasa ne okinchilis.";
    FP_MODES_DESCR[3] = "Otkritaya smena, 24 chasa zakonchilis.";
    FP_MODES_DESCR[4] = "Zakritaya smena.";
    FP_MODES_DESCR[5] = "Blokirovka po nepravilnomu parolyu nalogovogo inspektora.";
    FP_MODES_DESCR[6] = "Ojidaniye podtverjdeniya vvoda dati.";
    FP_MODES_DESCR[7] = "Razresheniye izmeneniya polojeniya desyatichnoy tochki.";
    FP_MODES_DESCR[8] = "Otkritiy dokument.";
    FP_MODES_DESCR[9] = "Rejim razresheniya tehnologicheskogo obnovleniya.";
    FP_MODES_DESCR[10] = "Testoviy progon";
    FP_MODES_DESCR[11] = "Pechat polnogo fis. otcheta.";
    FP_MODES_DESCR[12] = "Pechat otcheta EKLZ.";
    FP_MODES_DESCR[13] = "Rabota s fiskalnym podkladnym dokumentom.";
    FP_MODES_DESCR[14] = "Pechat podkladnogo dokumenta.";
    FP_MODES_DESCR[15] = "Fiskalniy podkladnoy dokument sformirovan.";
    if (mode >= 0 && mode <= 15)
        cout << mode << " - " << FP_MODES_DESCR[mode] << endl;
    else
        cout << mode << " - " << "Undefined mode." << endl;
}

int main(int argc, char *argv[])
{
    if (argc < 2) {
        show_usage(argv[0]);
        return 1;
    }

    string arg = argv[1];

    if ((arg == "-h") || (arg == "--help")) {
       show_usage(argv[0]);
       return 0;
    }

    settings Settings;
    Settings.initialize();
    custom shtrih(Settings.comport_name_shtrih, Settings.comport_baudrate_shtrih);

    if ((arg == "-z") || (arg == "--z-report")) {
        shtrih.reportZclose();
    } else if ((arg == "-x") || (arg == "--x-report")) {
        shtrih.printXreport();
    } else if ((arg == "-s") || (arg == "--status")) {
        shtrih.statusRequest();
    } else if ((arg == "-m") || (arg == "--mode")) {
        cout << shtrih.statusMode() << endl;
    } else if ((arg == "-t") || (arg == "--text")) {
        show_human_mode(shtrih.statusMode());
    } else if ((arg == "-n") || (arg == "--null-check")) {
        shtrih.avoidCheck();
    } else if ((arg == "-c") || (arg == "--close-check")) {
        string arg1 = argv[2];
        int price = atoi(arg1.c_str());
        shtrih.closeReceipt(price, 0, "1");
    } else if ((arg == "-o") || (arg == "--open-check")) {
        string arg1 = argv[2];
        int price = atoi(arg1.c_str());
        shtrih.Sale(1, price, "1");
    } else if ((arg == "-p") || (arg == "--poditog")) {
        shtrih.poditog();
    } else if (arg == "-l") {
        shtrih.testReceipt();
    } else if (arg == "-ps") {
        shtrih.PrintString("Netco Telecom");
    }

    return 0;
}


