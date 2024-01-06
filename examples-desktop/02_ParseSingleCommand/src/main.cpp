
#include <iostream>
#include <queue>
#include <string>

#include "GCGP.h"

std::queue<char> &GetGlobalSerialBuffer()
{
    static std::queue<char> buffer;
    return buffer;
}

int arduinoAvailable()
{
    return GetGlobalSerialBuffer().size();
}

int arduinoRead()
{
    if (GetGlobalSerialBuffer().empty()) {
        return -1;
    }
    int c = GetGlobalSerialBuffer().front();
    GetGlobalSerialBuffer().pop();
    return c;
}

int arduinoPeek()
{
    if (GetGlobalSerialBuffer().empty()) {
        return -1;
    }
    return GetGlobalSerialBuffer().front();
}

void arduinoWrite(const char *str)
{
    // std::cout << "[serial receive] << " << str;
    std::cout << "\033[1;32m" << str << "\033[0m";
}

void arduinoPrintln(const char *str)
{
    // std::cout << "[serial receive] << " << str << std::endl;
    std::cout << "\033[1;32m" << str << "\033[0m" << std::endl;
}

void serialWrite(const char *str)
{
    // std::cout << "[write serial] >> " << str;
    std::cout << "\033[1;34m" << str << "\033[0m";
    for (size_t i = 0; i < strlen(str); i++) {
        GetGlobalSerialBuffer().push(str[i]);
    }
}

void serialPrintln(const char *str)
{
    // std::cout << "[println serial] >> " << str << std::endl;
    std::cout << "\033[1;34m" << str << "\033[0m" << std::endl;
    for (size_t i = 0; i < strlen(str); i++) {
        GetGlobalSerialBuffer().push(str[i]);
    }
    GetGlobalSerialBuffer().push('\n');
}

class Tester {
  public:
    SerialInterface serial;
    GrblInterface grbl;

    Tester()
        : serial(arduinoAvailable, arduinoPeek, arduinoRead, arduinoWrite,
                 arduinoPrintln),
          grbl(serial, this)
    {
        grbl.cbGetFeedrate = [](void *instance) {
            return ((Tester*)instance)->feedrate;
        };
        grbl.cbIsInAlarmState = [](void *instance) {
            return false;
        };
        grbl.cbIsInJogState = [](void *instance) {
            return false;
        };
        grbl.cbIsIdle = [](void *instance) {
            return true;
        };
        grbl.cbCycleStart = [](void *instance) {
            std::cout << "Cycle Start" << std::endl;
        };
        grbl.cbFeedHold = [](void *instance) {
            std::cout << "Feed Hold" << std::endl;
        };
        grbl.cbSoftReset = [](void *instance) {
            std::cout << "Soft Reset" << std::endl;
        };
        grbl.cbBufferIsFull = [](void *instance) {
            return false;
        };
        grbl.cbProcessCommand = [](void *instance, Command<10>* command) {
            command->printContent(((Tester*)instance)->serial);
        };
    }

    void processLine(const std::string &line)
    {
        // std::cout << "Processing line: '" << line << "'" << std::endl;
        serialWrite(line.c_str());
        serialWrite("\n");
        for (int i = 0; i < 100; i++) {
            grbl.update();
        }
    }

    float feedrate = NAN;

  private:
    size_t m_framecount = 0;
    size_t m_nextLine = 0;
};

int main(int argc, char *argv[])
{
    std::cout << "Please enter a line of G-Code. (or 'quit')" << std::endl;
    Tester tester;

    while (true) {
        std::string line;
        std::cout << " > ";
        std::getline(std::cin, line);

        if (line == "quit") {
            break;
        }
        tester.processLine(line);
    }
}
