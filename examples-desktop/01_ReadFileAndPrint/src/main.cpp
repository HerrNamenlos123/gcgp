
#include <fstream>
#include <vector>
#include <string>
#include <thread>
#include <iostream>
#include <queue>
#include <functional>

#include "GCGP.h"

std::queue<char>& GetGlobalSerialBuffer() {
    static std::queue<char> buffer;
    return buffer;
}

int arduinoAvailable() {
    return GetGlobalSerialBuffer().size();
}

int arduinoRead() {
    if (GetGlobalSerialBuffer().empty()) {
        return -1;
    }
    int c = GetGlobalSerialBuffer().front();
    GetGlobalSerialBuffer().pop();
    return c;
}

int arduinoPeek() {
    if (GetGlobalSerialBuffer().empty()) {
        return -1;
    }
    return GetGlobalSerialBuffer().front();
}

void arduinoWrite(const char* str) {
    // std::cout << "[serial receive] << " << str;
    std::cout << "\033[1;32m" << str << "\033[0m";
}

void arduinoPrintln(const char* str) {
    // std::cout << "[serial receive] << " << str << std::endl;
    std::cout << "\033[1;32m" << str << "\033[0m" << std::endl;
}

void serialWrite(const char* str) {
    // std::cout << "[write serial] >> " << str;
    std::cout << "\033[1;34m" << str << "\033[0m";
    for (size_t i = 0; i < strlen(str); i++) {
        GetGlobalSerialBuffer().push(str[i]);
    }
}

void serialPrintln(const char* str) {
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
    GCGP parser;

    Tester() : serial(
        arduinoAvailable,
        arduinoPeek,
        arduinoRead,
        arduinoWrite,
        arduinoPrintln
    ),
    parser(serial) {}

    void processLine(const std::string& line) {
        // std::cout << "Processing line: '" << line << "'" << std::endl;
        serialWrite(line.c_str());
        serialWrite("\n");
    }

    int processFile(const std::vector<std::string>& lines) {
        while (true) {
            parser.update();

            if (m_framecount % 100 == 0 && m_nextLine < lines.size()) {
                processLine(lines[m_nextLine++]);
            }
            if (m_nextLine >= lines.size()) {
                std::cout << "Reached end of file" << std::endl;
                break;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            m_framecount++;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));
        return 0;
    }

    int serialAvailable() {

    }

    char serialPeek() {

    }

private:
    size_t m_framecount = 0;
    size_t m_nextLine = 0; 
};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: 01_ReadFileAndPrint <filename>\n");
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        printf("Could not open file %s\n", argv[1]);
        return 1;
    }

    std::vector<std::string> lines;
    std::string line;
    while (std::getline(file, line)) {
        lines.push_back(line);
    }

    file.close();

    Tester tester;
    return tester.processFile(lines);
}
