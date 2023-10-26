
#include "GCGP/TokenizedCommand.h"

TokenizedCommand::TokenizedCommand() {
	clear();
}

TokenizedCommand::TokenizedCommand(const StringView& str) {
    tokenize(str);
}

bool TokenizedCommand::getString(StringView& str) const {
    if (!isValid()) return false;
    
    size_t index = 0;
    if (index >= str.length()) return false;
	if (isSystemCommand()) {
        str[index++] = '$';
        if (index >= str.length()) return false;
        if (m_systemCommandLetter != 0) {
            str[index++] = m_systemCommandLetter;
            if (index >= str.length()) return false;
        }
        if (m_systemCommandIndex != -1) {
            str[index++] = m_systemCommandIndex + '0';
            if (index >= str.length()) return false;
        }
        if (m_systemCommandValueLetter != 0 || !isnan(m_systemCommandValue)) {
            str[index++] = '=';
            if (index >= str.length()) return false;
            if (m_systemCommandValueLetter != 0) {
                str[index++] = m_systemCommandValueLetter;
                if (index >= str.length()) return false;
            }
            if (!isnan(m_systemCommandValue)) {
                index += snprintf(str.data() + index, str.length() - index, "%d.%06d", 
                                  static_cast<uint32_t>(floor((double)m_systemCommandValue)), 
                                  static_cast<uint32_t>(floor((double)m_systemCommandValue * 1000000)) % 1000000);
                if (index >= str.length()) return false;
            }
        }
    }
    for (size_t i = 0; i < m_numTokens; i++) {
        str[index++] = m_tokenType[i];
        if (index >= str.length()) return false;
        index += snprintf(str.data() + index, str.length() - index, "%d.%06d",
                          static_cast<uint32_t>(floor((double)m_tokenValue[i])), 
                          static_cast<uint32_t>(floor((double)m_tokenValue[i] * 1000000)) % 1000000);
        if (index >= str.length()) return false;
        if (i < m_numTokens - 1) {
            str[index++] = ' ';
            if (index >= str.length()) return false;
        }
    }
    str[index] = '\0';
    return true;
}

enum class InterpreterState {
    EXPECT_LETTER,
    EXPECT_NUMBER,
    EXPECT_DECIMALS,
    EXPECT_SYSTEM_CMD_LETTER,
    EXPECT_SYSTEM_CMD_INDEX,
    EXPECT_SYSTEM_CMD_EQUALS,
    EXPECT_SYSTEM_CMD_VALUE,
    EXPECT_SYSTEM_CMD_VALUE_DECIMALS,
};

static bool inCapitalAlphabet(char c) {
    return c >= 'A' && c <= 'Z';
}

static bool isDigit(char c) {
    return c >= '0' && c <= '9';
}

static int charToInt(char c) {
    return c - '0';
}

bool TokenizedCommand::tokenize(const StringView& str) {
    clear();

    // This function parses a command into tokens
    // Example:
    //    G1 X10 Y20.78 Z-30 F56000.0 S-.5
    // Every command consists of a letter and a value. 
    // Spaces between tokens do not matter.
    // The example would be parsed and stored as tokens like this:
    //    m_tokenType = { 'G', 'X', 'Y', 'Z', 'F', 'S' }
    //    m_tokenValue = { 1.f, 10.f, 20.78f, -30.f, 56000.f, -0.5f }
    //
    // Or, if the first character is a '$', it is a system command:
    //
    //    '$G'
    //    m_systemCommandLetter = 'G'
    //    m_systemCommandIndex = -1
    //    m_systemCommandValue = NAN
    //    m_systemCommandValueLetter = 0
    //
    //    '$$'
    //    m_systemCommandLetter = '$'
    //    m_systemCommandIndex = -1
    //    m_systemCommandValue = NAN
    //    m_systemCommandValueLetter = 0
    //
    //    '$13=0.5'
    //    m_systemCommandLetter = 0
    //    m_systemCommandIndex = 13
    //    m_systemCommandValue = 0.5f
    //    m_systemCommandValueLetter = 0
    //
    //    '$N0=G1 X10'
    //    m_systemCommandLetter = 'N'
    //    m_systemCommandIndex = 0
    //    m_systemCommandValue = NAN
    //    m_systemCommandValueLetter = 0
    //    m_tokenType = { 'G', 'X' }
    //    m_tokenValue = { 1.f, 10.f }
    //
    //    '$SLP' and '$RST=$' are the only exceptions: They are the only
    //    system commands with more than one letter, so they are stored 
    //    as 'S' and 'R' respectively, as these are not valid commands on their own.
    //
    //    '$SLP'
    //    m_systemCommandLetter = 'S'
    //    m_systemCommandIndex = -1
    //    m_systemCommandValue = NAN
    //    m_systemCommandValueLetter = 0
    //
    //    '$RST=$', '$RST=#' and '$RST=*'
    //    m_systemCommandLetter = 'R'
    //    m_systemCommandIndex = -1
    //    m_systemCommandValue = NAN
    //    m_systemCommandValueLetter = '$' or '#' or '*'
    //
    // A command is a system command, if either m_systemCommandLetter or m_systemCommandIndex is set.
    // m_systemCommandValueLetter is used for '$RST=...' exclusively.
    //

    enum class InterpreterState state = InterpreterState::EXPECT_LETTER;
    float decimalPlace = 0.1f; // This must be reset whenever switching to EXPECT_DECIMALS (going 0.1, 0.01, 0.001, ...)
    bool isNegative = false;   // This must be reset whenever switching to EXPECT_NUMBER
    bool haveNumber = false;   // This must be reset whenever switching to EXPECT_NUMBER

	for (size_t i = 0; i < str.length(); i++) {
		char c = str[i];

        switch (state) {

        // =============================================== EXPECT_LETTER ========================================================
        case InterpreterState::EXPECT_LETTER:
            if (c == '$' && m_numTokens == 0) {  // It is a system command
                state = InterpreterState::EXPECT_SYSTEM_CMD_LETTER;
            }
            else if (inCapitalAlphabet(c)) {     // It is a letter, store it and expect a number now
                m_tokenType[m_numTokens] = c;
                state = InterpreterState::EXPECT_NUMBER;
                isNegative = false;
                haveNumber = false;
            }
            else if (c == ' ') {                 // Spaces are ignored
                // Do nothing
            }
            else {                               // Not a capital letter, space or system command: Error
                clear();
                m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                return false;
            }
            break;

        // =============================================== EXPECT_NUMBER ========================================================
        case InterpreterState::EXPECT_NUMBER:    // Expect integer part of a number

            if (isDigit(c)) {                    // It is a digit, append it to the value
                m_tokenValue[m_numTokens] = m_tokenValue[m_numTokens] * 10 + charToInt(c);
                haveNumber = true;
            }
            else if (c == '.') {                 // Expect floating point digits now
                state = InterpreterState::EXPECT_DECIMALS;
                decimalPlace = 0.1f;
            }
            else if (c == '-') {                 // Mark to later negate the value
                if (!haveNumber && !isNegative) {
                    isNegative = true;
                }
                else {
                    clear();
                    m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                    return false;
                }
            }
            else if (inCapitalAlphabet(c)) {    // Another token is started
                if (haveNumber) {
                    m_tokenValue[m_numTokens] *= isNegative ? -1.f : 1.f;
                    m_numTokens++;

                    m_tokenType[m_numTokens] = c;
                    state = InterpreterState::EXPECT_NUMBER;
                    isNegative = false;
                    haveNumber = false;
                }
                else {
                    clear();
                    m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                    return false;
                }
            }
            else if (c == ' ') {                // Token is ended by space
                if (haveNumber) {
                    m_tokenValue[m_numTokens] *= isNegative ? -1.f : 1.f;
                    m_numTokens++;
                    state = InterpreterState::EXPECT_LETTER;
                }
                else {
                    clear();
                    m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                    return false;
                }
            }
            else {
                clear();
                m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                return false;
            }
            break;

        // =============================================== EXPECT_DECIMALS ========================================================
        case InterpreterState::EXPECT_DECIMALS:  // decimal places of a floating point number

            if (isDigit(c)) {
                m_tokenValue[m_numTokens] += charToInt(c) * decimalPlace;
                decimalPlace *= 0.1f;
                haveNumber = true;
            }
            else if (inCapitalAlphabet(c)) {     // Another token started
                if (haveNumber) {
                    m_tokenValue[m_numTokens] *= isNegative ? -1.f : 1.f;
                    m_numTokens++;

                    m_tokenType[m_numTokens] = c;
                    state = InterpreterState::EXPECT_NUMBER;
                    isNegative = false;
                    haveNumber = false;
                }
                else {
                    clear();
                    m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                    return false;
                }
            }
            else if (c != ' ') {                 // Token was ended by space
                if (haveNumber) {
                    m_tokenValue[m_numTokens] *= isNegative ? -1.f : 1.f;
                    m_numTokens++;
                    state = InterpreterState::EXPECT_LETTER;
                }
                else {
                    clear();
                    m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                    return false;
                }
            }
            else {
                clear();
                m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                return false;
            }
            break;

        // =============================================== EXPECT_SYSTEM_CMD_LETTER ========================================================
        case InterpreterState::EXPECT_SYSTEM_CMD_LETTER:
            if (inCapitalAlphabet(c) || c == '$' || c == '#') {         // It is a letter, store it and expect an integer index now (Example $N0=)
                if (c == 'S') {                      // $SLP is a 3-character exception
                    if (i < str.length() - 2) {
                        if (str[i + 1] == 'L' && str[i + 2] == 'P') {
                            m_systemCommandLetter = 'S';
                            state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
                            i += 2;
                        }
                    }
                    else {
                        clear();
                        m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                        return false;
                    }
                }
                else if (c == 'R') {                 // $RST is a 3-character exception  
                    if (i < str.length() - 2) {
                        if (str[i + 1] == 'S' && str[i + 2] == 'T') {
                            m_systemCommandLetter = 'R';
                            state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
                            i += 2;
                        }
                    }
                    else {
                        clear();
                        m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                        return false;
                    }
                }
                else {
                    m_systemCommandLetter = c;
                    state = InterpreterState::EXPECT_SYSTEM_CMD_INDEX;
                }
            }
            else if (isDigit(c)) {                                      // $ is followed by a number (Example $10=)
                m_systemCommandIndex = charToInt(c);
                state = InterpreterState::EXPECT_SYSTEM_CMD_INDEX;
            }
            else if (c == ' ') {                                        // Space between letter and number is not allowed, we need '=' now
                state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
            }
            else {
                clear();
                m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                return false;
            }
            break;

        // =============================================== EXPECT_SYSTEM_CMD_INDEX ========================================================
        case InterpreterState::EXPECT_SYSTEM_CMD_INDEX:
            if (isDigit(c)) {                                      // Enlarge the number (Example either '$N10=' or '$10=')
                m_systemCommandIndex = m_systemCommandIndex * 10 + charToInt(c);
            }
            else if (c == '=') {
                state = InterpreterState::EXPECT_SYSTEM_CMD_VALUE;
                isNegative = false;
                haveNumber = false;
            }
            else if (c == ' ') {                                        // Space ends the index number
                state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
            }
            else {
                clear();
                m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                return false;
            }
            break;

        // =============================================== EXPECT_SYSTEM_CMD_EQUALS ========================================================
        case InterpreterState::EXPECT_SYSTEM_CMD_EQUALS:
            if (c == '=') {
                state = InterpreterState::EXPECT_SYSTEM_CMD_VALUE;
                isNegative = false;
                haveNumber = false;
            }
            else if (c == ' ') {        // Wait for '='
                // Do nothing
            }
            else {
                clear();
                m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                return false;
            }
            break;

        // =============================================== EXPECT_SYSTEM_CMD_VALUE ========================================================
        case InterpreterState::EXPECT_SYSTEM_CMD_VALUE:         // Now we can either find a number, or a full G-Code command
            if (inCapitalAlphabet(c) && !haveNumber) {          // It is a G-Code command, continue like a normal command
                m_tokenType[m_numTokens] = c;
                state = InterpreterState::EXPECT_NUMBER;
                isNegative = false;
                haveNumber = false;
            }
            else if (c == ' ' && !haveNumber) {                 // Space after the '=' is allowed, not in the value
                // Do nothing
            }
            else if (m_systemCommandLetter == 'R' && c == '$') {  // Special case $RST=$
                m_systemCommandValueLetter = '$';
            }
            else if (m_systemCommandLetter == 'R' && c == '#') {  // Special case $RST=#
                m_systemCommandValueLetter = '#';
            }
            else if (m_systemCommandLetter == 'R' && c == '*') {  // Special case $RST=*
                m_systemCommandValueLetter = '*';
            }
            else if (isDigit(c)) {                              // It is a digit, append it to the value
                m_systemCommandValue = m_systemCommandValue * 10 + charToInt(c);
                haveNumber = true;
            }
            else if (c == '.') {                                // Expect floating point digits now
                state = InterpreterState::EXPECT_SYSTEM_CMD_VALUE_DECIMALS;
                decimalPlace = 0.1f;
            }
            else if (c == '-') {                 // Mark to later negate the value
                if (!haveNumber && !isNegative) {
                    isNegative = true;
                }
                else {
                    clear();
                    m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                    return false;
                }
            }
            break;

        // =============================================== EXPECT_SYSTEM_CMD_VALUE_DECIMALS ========================================================
        case InterpreterState::EXPECT_SYSTEM_CMD_VALUE_DECIMALS:
            if (isDigit(c)) {
                m_systemCommandValue += charToInt(c) * decimalPlace;
                decimalPlace *= 0.1f;
                haveNumber = true;
            }
            else {
                clear();
                m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
                return false;
            }
            break;
        }
	}

    // Finish the state off
    switch (state) {
        case InterpreterState::EXPECT_LETTER:   // This is fine
            break;

        case InterpreterState::EXPECT_NUMBER:       // This is fine if we have a number
        case InterpreterState::EXPECT_DECIMALS:     // This is fine if we have a number
            if (haveNumber) {
                m_tokenValue[m_numTokens] *= isNegative ? -1.f : 1.f;
            }
            else {
                clear();
                m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                return false;
            }
            break;

        case InterpreterState::EXPECT_SYSTEM_CMD_LETTER:    // This is never fine
            clear();
            m_errorCode = GrblError::UnsupportedOrInvalidGCodeCommand;
            return false;

        case InterpreterState::EXPECT_SYSTEM_CMD_INDEX:     // This is fine (Example: '$G')
        case InterpreterState::EXPECT_SYSTEM_CMD_EQUALS:    // This is fine (Example: '$G10')
            break;

        case InterpreterState::EXPECT_SYSTEM_CMD_VALUE:             // This is fine if we have a number
        case InterpreterState::EXPECT_SYSTEM_CMD_VALUE_DECIMALS:    // This is fine if we have a number
            if (haveNumber) {
                m_systemCommandValue *= isNegative ? -1.f : 1.f;
            }
            else {
                clear();
                m_errorCode = GrblError::GCodeCommandValueInvalidOrMissing;
                return false;
            }
            break;
        default:
            break;
    }

    m_isValid = true;
    return true;
}

bool TokenizedCommand::isSystemCommand() const {
    return m_systemCommandLetter != 0 || m_systemCommandIndex != -1;
}

void TokenizedCommand::clear() {
	m_numTokens = 0;
    m_errorCode = GrblError::None;
    m_isValid = false;

    m_systemCommandLetter = 0;
    m_systemCommandIndex = -1;
    m_systemCommandValue = NAN;
    m_systemCommandValueLetter = 0;

	for (size_t i = 0; i < GCGP_MAX_NUM_OF_CMD_TOKENS; i++) {
		m_tokenType[i] = 0;
		m_tokenValue[i] = 0;
	}
}
