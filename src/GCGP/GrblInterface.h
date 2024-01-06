#ifdef __cplusplus
#ifndef GCGP_GRBLINTERFACE_H
#define GCGP_GRBLINTERFACE_H

#include "GCGP/Command.h"
#include "GCGP/Config.h"
#include "GCGP/Serial.h"
#include "GCGP/String.h"

/// @brief Class for implementing GRBL-compatible communication with the host.
/// @details This class is responsible for reading the serial stream byte by byte,
///          parsing and interpreting them, and responding how GRBL would.
class GrblInterface {
  public:
    bool (*cbBufferIsFull)(void *) = nullptr;
    void (*cbProcessCommand)(void *, Command<10> *) = nullptr;
    void (*cbCycleStart)(void *) = nullptr;
    void (*cbFeedHold)(void *) = nullptr;
    void (*cbSoftReset)(void *) = nullptr;
    bool (*cbIsIdle)(void *) = nullptr;
    bool (*cbIsInAlarmState)(void *) = nullptr;
    bool (*cbIsInJogState)(void *) = nullptr;
    float (*cbGetFeedrate)(void *) = nullptr;

    GrblInterface(const SerialInterface &serialInterface, void *instance);

    void update();

  private:
    void parseSingleByte();
    void flushSerialCommand(); // Flush the current command, until a newline is found.

    void printStatusReport();
    void printOk();
    void printError(const char *error);
    void printError(GrblError error);

    void appendCharacter(char c);
    void finishCommand();
    void processCommand(const char *command);
    void processSystemCommand(const char *command);
    void processGCodeCommand(const char *command);

    SerialInterface m_serial;
    size_t m_commandBufferIndex = 0;
    char m_commandBuffer[GCGP_MAX_COMMAND_LENGTH];
    Command<10> m_command;

    void *m_instance = nullptr;
};

#endif // GCGP_GRBLINTERFACE_H
#endif // __cplusplus
