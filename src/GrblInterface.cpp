
#include "GCGP/GrblInterface.h"

GrblInterface::GrblInterface(const SerialInterface& serialInterface) : m_serial(serialInterface) {
    m_serial.println(GCGP_WELCOME_MESSAGE);
}

void GrblInterface::update() {

    // If we see there is an immediate command pending, handle it (even if buffer is full)
    if (m_serial.peek() == GCGP_CMD_CYCLE_START || 
        m_serial.peek() == GCGP_CMD_FEED_HOLD || 
        m_serial.peek() == GCGP_CMD_SOFT_RESET || 
        m_serial.peek() == GCGP_CMD_STATUS_REPORT) 
    {
        readByte();
    }

    // if (queueFull) {
    //     return;
    // }

	while (/*!queueFull &&*/ m_serial.available() > 0) {
		readByte();
	}
}

static bool isPrintableCharacter(char c) {
    return c >= 32 && c <= 126;
}

void GrblInterface::readByte() {
	char c = m_serial.read();
	switch (c) {

        case -1:	// No new data: do nothing
            break;

        case GCGP_CMD_STATUS_REPORT:
            printStatusReport();
            break;

        // case GCGP_CMD_FEED_HOLD:
        //     OnFeedHold();
        //     break;

        // case GCGP_CMD_CYCLE_START:
        //     OnCycleStart();
        //     break;

        // case GCGP_CMD_SOFT_RESET:
        //     OnCycleCancel();
        //     break;

        case '\r':    // Carriage return is ignored, Line feed is crucial
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
                snprintf(str, 64, "Unknown character: (%i): '%c'", (int)c, c);
                printError(str);
                flushSerialCommand();
            }
            break;
	}
}

void GrblInterface::flushSerialCommand() {
    char c = '\0';
    do {
        c = m_serial.read();
    } while (c != '\n' && c != -1);
}

void GrblInterface::printStatusReport() {

	// glm::vec3 desired = GetDesiredPosition();
	// glm::vec3 actual = GetActualPosition();

	// char str[100];
	// snprintf(str, 100, "<%s,MPos:%d.%03d,%d.%03d,%d.%03d,WPos:%d.%03d,%d.%03d,%d.%03d,F:%d>", GetStateString(),
	// 	(int)desired.x, (int)(fabsf(desired.x) * 1000) % 1000,
	// 	(int)desired.y, (int)(fabsf(desired.y) * 1000) % 1000,
	// 	(int)desired.z, (int)(fabsf(desired.z) * 1000) % 1000,
	// 	(int)actual.x, (int)(fabsf(actual.x) * 1000) % 1000,
	// 	(int)actual.y, (int)(fabsf(actual.y) * 1000) % 1000,
	// 	(int)actual.z, (int)(fabsf(actual.z) * 1000) % 1000,
	// 	(int)GetFeedrate()
	// );
	// Serial.println(str);

}

void GrblInterface::printOk() {
    m_serial.println(GCGP_OK_MESSAGE);
}

void GrblInterface::printError(const StringView& error) {
    m_serial.write(GCGP_ERROR_MESSAGE);
    m_serial.println(error.data());
}

void GrblInterface::appendCharacter(char c) {
    m_commandBuffer[m_commandBufferIndex++] = c;
    if (m_commandBufferIndex >= GCGP_MAX_COMMAND_LENGTH) {
        m_commandBufferIndex = 0;
        memset(m_commandBuffer, 0, GCGP_MAX_COMMAND_LENGTH);
        char str[64];
        snprintf(str, 64, "Command too long (>%d)", GCGP_MAX_COMMAND_LENGTH);
        printError(str);
        flushSerialCommand();
    }
}

void GrblInterface::finishCommand() {
    m_commandBuffer[m_commandBufferIndex] = '\0';
    m_commandBufferIndex = 0;
    if (m_commandBuffer[0] != '\0') {
        processCommand(m_commandBuffer);
    }
}

void GrblInterface::processCommand(const StringView& command) {
    m_serial.write("Processing command: ");
    m_serial.println(command.data());

    

    if (command[0] == GCGP_SYSTEM_CMD) {
        processSystemCommand(command);
    }
    else {
        processGCodeCommand(command);
    }
}

void GrblInterface::processSystemCommand(const StringView& command) {
    if (command[0] != GCGP_SYSTEM_CMD) {
        return;
    }

    // switch (command[1]) {
    //     case 
    // }
}

void GrblInterface::processGCodeCommand(const StringView& command) {
    
}
