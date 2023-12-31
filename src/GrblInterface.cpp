
#include "GCGP/GrblInterface.h"

GrblInterface::GrblInterface(const SerialInterface &serialInterface, void *instance)
    : m_serial(serialInterface), m_instance(instance)
{
    m_serial.println(GCGP_WELCOME_MESSAGE);
}

// This function reads the serial interface as long as there is data, and
// stops when an entire command has been parsed. Then, the command is parsed,
// a callback is called, the command is removed and the function returns.
void GrblInterface::update()
{
    // If we see there is an immediate command pending, handle it (even if buffer is full)
    if (m_serial.peek() == GCGP_CMD_CYCLE_START ||
        m_serial.peek() == GCGP_CMD_FEED_HOLD || m_serial.peek() == GCGP_CMD_SOFT_RESET ||
        m_serial.peek() == GCGP_CMD_STATUS_REPORT) {
        parseSingleByte();
    }

    bool bufferFull = false;
    if (cbBufferIsFull) {
        bufferFull = cbBufferIsFull(m_instance);
    }

    if (!bufferFull) {
        while (m_serial.available() > 0) {
            parseSingleByte();
        }
    }
}

static bool isPrintableCharacter(char c)
{
    return c >= 32 && c <= 126;
}

void GrblInterface::parseSingleByte()
{
    char c = m_serial.read();
    switch (c) {
        case -1: // No new data: do nothing
            break;

        case GCGP_CMD_STATUS_REPORT:
            printStatusReport();
            break;

        case GCGP_CMD_FEED_HOLD:
            if (cbFeedHold) {
                cbFeedHold(m_instance);
            }
            break;

        case GCGP_CMD_CYCLE_START:
            if (cbCycleStart) {
                cbCycleStart(m_instance);
            }
            break;

        case GCGP_CMD_SOFT_RESET:
            if (cbCycleStart) {
                cbCycleStart(m_instance);
            }
            break;

        case '\r': // Carriage return is ignored, Line feed is crucial
            break;

        case '\n':
            finishCommand();
            break;

        default:
            if (isPrintableCharacter(c)) {
                appendCharacter(c);
            }
            else {
                char str[64];
                snprintf(str, sizeof(str), "Unknown character: 0x%x", (int)c);
                printError(str);
                flushSerialCommand(); // Get rid of rest of command
            }
            break;
    }
}

void GrblInterface::flushSerialCommand()
{
    char c = '\0';
    do {
        c = m_serial.read();
    } while (c != '\n' && c != -1);
}

void GrblInterface::printStatusReport()
{

    // glm::vec3 desired = GetDesiredPosition();
    // glm::vec3 actual = GetActualPosition();

    // char str[100];
    // snprintf(str, 100,
    // "<%s,MPos:%d.%03d,%d.%03d,%d.%03d,WPos:%d.%03d,%d.%03d,%d.%03d,F:%d>",
    // GetStateString(), 	(int)desired.x, (int)(fabsf(desired.x) * 1000) % 1000,
    // 	(int)desired.y, (int)(fabsf(desired.y) * 1000) % 1000,
    // 	(int)desired.z, (int)(fabsf(desired.z) * 1000) % 1000,
    // 	(int)actual.x, (int)(fabsf(actual.x) * 1000) % 1000,
    // 	(int)actual.y, (int)(fabsf(actual.y) * 1000) % 1000,
    // 	(int)actual.z, (int)(fabsf(actual.z) * 1000) % 1000,
    // 	(int)GetFeedrate()
    // );
    // Serial.println(str);
}

void GrblInterface::printOk()
{
    m_serial.println(GCGP_OK_MESSAGE);
}

void GrblInterface::printError(const char *error)
{
    m_serial.write(GCGP_ERROR_MESSAGE);
    m_serial.println(error);
}

void GrblInterface::printError(GrblError error)
{
    printError(ErrorEnumToString(error));
}

void GrblInterface::appendCharacter(char c)
{
    m_commandBuffer[m_commandBufferIndex++] = c;
    if (m_commandBufferIndex >= GCGP_MAX_COMMAND_LENGTH) {
        m_commandBufferIndex = 0;
        memset(m_commandBuffer, 0, GCGP_MAX_COMMAND_LENGTH);
        char str[64];
        snprintf(str, sizeof(str), "Command too long (>%d)", GCGP_MAX_COMMAND_LENGTH);
        printError(str);
        flushSerialCommand();
    }
}

void GrblInterface::finishCommand()
{
    m_commandBuffer[m_commandBufferIndex] = '\0';
    m_commandBufferIndex = 0;
    if (m_commandBuffer[0] != '\0') {
        processCommand(m_commandBuffer);
    }
}

void GrblInterface::processCommand(const char *command)
{
    auto error = m_command.parse(command, strlen(command));
    if (error != GrblError::None) {
        printError(error);
        return;
    }

    if (m_command.isSystemCommand) {
        if (cbIsIdle) {
            if (!cbIsIdle(m_instance)) {
                printError(ErrorEnumToString(GrblError::GrblSystemCmdOnlyValidWhenIdle));
                return;
            }
        }
    }

    if (m_command.isGCode && m_command.isMCode) {
        bool disallowed = false;
        if (cbIsInAlarmState) {
            if (cbIsInAlarmState(m_instance)) {
                disallowed = true;
            }
        }
        if (cbIsInJogState) {
            if (cbIsInJogState(m_instance)) {
                disallowed = true;
            }
        }
        if (disallowed) {
            printError(
                ErrorEnumToString(GrblError::GCodeCommandsInvalidInAlarmOrJogState));
        }
    }

    if (m_command.motionType != MotionType::None && isnan(m_command.setFeedrate)) {
        if (cbGetFeedrate) {
            if (isnan(cbGetFeedrate(m_instance))) {
                printError(
                    ErrorEnumToString(GrblError::FeedRateHasNotYetBeenSetOrIsNone));
            }
        }
    }

    if (cbProcessCommand) {
        cbProcessCommand(m_instance, &m_command);
    }
    printOk();
}
