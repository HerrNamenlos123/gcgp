#ifdef __cplusplus
#ifndef GCGP_GRBLINTERFACE_H
#define GCGP_GRBLINTERFACE_H

#include "GCGP/Serial.h"
#include "GCGP/Config.h"
#include "GCGP/String.h"

class GrblInterface {
public:
    GrblInterface(const SerialInterface& serialInterface);

    void update();

private:
    void readByte();
    void flushSerialCommand();  // Flush the current command, until a newline has been received.
    
    void printStatusReport();
    void printOk();
    void printError(const StringView& error);

    void appendCharacter(char c);
    void finishCommand();
    void processCommand(const StringView& command);
    void processSystemCommand(const StringView& command);
    void processGCodeCommand(const StringView& command);

    SerialInterface m_serial;
    size_t m_commandBufferIndex = 0;
    char m_commandBuffer[GCGP_MAX_COMMAND_LENGTH];
};

#endif // GCGP_GRBLINTERFACE_H
#endif // __cplusplus