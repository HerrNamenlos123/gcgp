#ifdef __cplusplus
#ifndef GCGP_TOKENIZEDCOMMAND_H
#define GCGP_TOKENIZEDCOMMAND_H

#include "GCGP/Config.h"
#include "GCGP/Enums.h"
#include "GCGP/String.h"

struct SystemCommand {
    char letter = '\0';
    int index = -1;
    float value = 0.f;
    char valueLetter = '\0';

    bool operator==(const SystemCommand &other) const
    {
        return letter == other.letter && index == other.index && value == other.value &&
               valueLetter == other.valueLetter;
    }
};

struct Token {
    char type = '\0';
    float value = 0.f;

    bool operator==(const Token &other) const
    {
        return type == other.type && value == other.value;
    }
};

template <size_t capacity> struct CommandTokens {
    Token tokens[capacity] = {};
    size_t numberOfValidTokens = 0;
    bool isSystemCommand = false;
    SystemCommand systemCommand;

    bool operator==(const CommandTokens<capacity> &other) const
    {
        for (int i = 0; i < capacity; i++) {
            if (tokens[i] != other.tokens[i]) {
                return false;
            }
        }
        return numberOfValidTokens == other.numberOfValidTokens &&
               isSystemCommand == other.isSystemCommand &&
               systemCommand == other.systemCommand;
    }
};

static bool inCapitalAlphabet(char c)
{
    return c >= 'A' && c <= 'Z';
}

static bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

static int charToInt(char c)
{
    return c - '0';
}

template <size_t capacity>
GrblError tokenizeCommand(CommandTokens<capacity> &tokens, const char *str,
                          size_t strLength)
{
    // This function parses a command into tokens
    // Example:
    //    G1 X10 Y20.78 Z-30 F56000.0 S-.5
    // Every command consists of a letter and a value.
    // Spaces between tokens do not matter.
    // The example would be parsed and stored as tokens like this:
    //    tokenType = { 'G', 'X', 'Y', 'Z', 'F', 'S' }
    //    tokenValue = { 1.f, 10.f, 20.78f, -30.f, 56000.f, -0.5f }
    //
    // Or, if the first character is a '$', it is a system command:
    //
    //    '$G'
    //    systemCommandLetter = 'G'
    //    systemCommandIndex = -1
    //    systemCommandValue = 0.f
    //    systemCommandValueLetter = 0
    //
    //    '$$'
    //    systemCommandLetter = '$'
    //    systemCommandIndex = -1
    //    systemCommandValue = 0.f
    //    systemCommandValueLetter = 0
    //
    //    '$13=0.5'
    //    systemCommandLetter = 0
    //    systemCommandIndex = 13
    //    systemCommandValue = 0.5f
    //    systemCommandValueLetter = 0
    //
    //    '$N0=G1 X10'
    //    systemCommandLetter = 'N'
    //    systemCommandIndex = 0
    //    systemCommandValue = 0.f
    //    systemCommandValueLetter = 0
    //    tokenType = { 'G', 'X' }
    //    tokenValue = { 1.f, 10.f }
    //
    //    '$SLP' and '$RST=$' are the only exceptions: They are the only
    //    system commands with more than one letter, so they are stored
    //    as 'S' and 'R' respectively, as these are not valid commands on their
    //    own.
    //
    //    '$SLP'
    //    systemCommandLetter = 'S'
    //    systemCommandIndex = -1
    //    systemCommandValue = 0.f
    //    systemCommandValueLetter = 0
    //
    //    '$RST=$', '$RST=#' and '$RST=*'
    //    systemCommandLetter = 'R'
    //    systemCommandIndex = -1
    //    systemCommandValue = 0.f
    //    systemCommandValueLetter = '$' or '#' or '*'
    //
    // A command is a system command, if either systemCommandLetter or
    // systemCommandIndex is set. systemCommandValueLetter is used for
    // '$RST=...' exclusively.
    //
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

    enum class InterpreterState state = InterpreterState::EXPECT_LETTER;
    float decimalPlace = 0.1f; // This must be reset whenever switching to
                               // EXPECT_DECIMALS (going 0.1, 0.01, 0.001, ...)
    bool isNegative = false;   // This must be reset whenever switching to EXPECT_NUMBER
    bool haveNumber = false;   // This must be reset whenever switching to EXPECT_NUMBER

    for (size_t i = 0; i < strLength; i++) {
        char c = str[i];

        switch (state) {

            // =============================================== EXPECT_LETTER
            case InterpreterState::EXPECT_LETTER:
                if (c == '$' &&
                    tokens.numberOfValidTokens == 0) { // It is a system command
                    state = InterpreterState::EXPECT_SYSTEM_CMD_LETTER;
                }
                else if (inCapitalAlphabet(c)) { // It is a letter, store it and
                                                 // expect a number now
                    if (tokens.numberOfValidTokens >= capacity) {
                        tokens = {};
                        return GrblError::GCodeTooManyParameters;
                    }
                    tokens.tokens[tokens.numberOfValidTokens].type = c;
                    state = InterpreterState::EXPECT_NUMBER;
                    isNegative = false;
                    haveNumber = false;
                }
                else if (c == ' ') { // Spaces are ignored
                                     // Do nothing
                }
                else { // Not a capital letter, space or system command: Error
                    tokens = {};
                    return GrblError::UnsupportedOrInvalidGCodeCommand;
                }
                break;

            // =============================================== EXPECT_NUMBER
            case InterpreterState::EXPECT_NUMBER: // Expect integer part of a number

                if (isDigit(c)) { // It is a digit, append it to the value
                    tokens.tokens[tokens.numberOfValidTokens].value =
                        tokens.tokens[tokens.numberOfValidTokens].value * 10 +
                        charToInt(c);
                    haveNumber = true;
                }
                else if (c == '.') { // Expect floating point digits now
                    state = InterpreterState::EXPECT_DECIMALS;
                    decimalPlace = 0.1f;
                }
                else if (c == '-') { // Mark to later negate the value
                    if (!haveNumber && !isNegative) {
                        isNegative = true;
                    }
                    else {
                        tokens = {};
                        return GrblError::GCodeCommandValueInvalidOrMissing;
                    }
                }
                else if (inCapitalAlphabet(c)) { // Another token is started
                    if (haveNumber) {
                        tokens.tokens[tokens.numberOfValidTokens].value *=
                            isNegative ? -1.f : 1.f;
                        tokens.numberOfValidTokens++;

                        if (tokens.numberOfValidTokens >= capacity) {
                            tokens = {};
                            return GrblError::GCodeTooManyParameters;
                        }
                        tokens.tokens[tokens.numberOfValidTokens].type = c;
                        state = InterpreterState::EXPECT_NUMBER;
                        isNegative = false;
                        haveNumber = false;
                    }
                    else {
                        tokens = {};
                        return GrblError::GCodeCommandValueInvalidOrMissing;
                    }
                }
                else if (c == ' ') { // Token is ended by space
                    if (haveNumber) {
                        tokens.tokens[tokens.numberOfValidTokens].value *=
                            isNegative ? -1.f : 1.f;
                        tokens.numberOfValidTokens++;
                        state = InterpreterState::EXPECT_LETTER;
                    }
                    else {
                        tokens = {};
                        return GrblError::GCodeCommandValueInvalidOrMissing;
                    }
                }
                else {
                    tokens = {};
                    return GrblError::UnsupportedOrInvalidGCodeCommand;
                }
                break;

            // =============================================== EXPECT_DECIMALS
            case InterpreterState::EXPECT_DECIMALS: // decimal places of a floating
                                                    // point number

                if (isDigit(c)) {
                    tokens.tokens[tokens.numberOfValidTokens].value +=
                        charToInt(c) * decimalPlace;
                    decimalPlace *= 0.1f;
                    haveNumber = true;
                }
                else if (inCapitalAlphabet(c)) { // Another token started
                    if (haveNumber) {
                        tokens.tokens[tokens.numberOfValidTokens].value *=
                            isNegative ? -1.f : 1.f;
                        tokens.numberOfValidTokens++;

                        if (tokens.numberOfValidTokens >= capacity) {
                            tokens = {};
                            return GrblError::GCodeTooManyParameters;
                        }
                        tokens.tokens[tokens.numberOfValidTokens].type = c;
                        state = InterpreterState::EXPECT_NUMBER;
                        isNegative = false;
                        haveNumber = false;
                    }
                    else {
                        tokens = {};
                        return GrblError::GCodeCommandValueInvalidOrMissing;
                    }
                }
                else if (c == ' ') { // Token was ended by space
                    if (haveNumber) {
                        tokens.tokens[tokens.numberOfValidTokens].value *=
                            isNegative ? -1.f : 1.f;
                        tokens.numberOfValidTokens++;
                        state = InterpreterState::EXPECT_LETTER;
                    }
                    else {
                        tokens = {};
                        return GrblError::GCodeCommandValueInvalidOrMissing;
                    }
                }
                else {
                    tokens = {};
                    return GrblError::UnsupportedOrInvalidGCodeCommand;
                }
                break;

            // ===============================================
            // EXPECT_SYSTEM_CMD_LETTER
            // ========================================================
            case InterpreterState::EXPECT_SYSTEM_CMD_LETTER:
                if (inCapitalAlphabet(c) || c == '$' ||
                    c == '#') {     // It is a letter, store it and expect an integer
                                    // index now (Example $N0=)
                    if (c == 'S') { // $SLP is a 3-character exception
                        if (i < strLength - 2) {
                            if (str[i + 1] == 'L' && str[i + 2] == 'P') {
                                tokens.systemCommand.letter = 'S';
                                state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
                                i += 2;
                            }
                        }
                        else {
                            tokens = {};
                            return GrblError::UnsupportedOrInvalidGCodeCommand;
                        }
                    }
                    else if (c == 'R') { // $RST is a 3-character exception
                        if (i < strLength - 2) {
                            if (str[i + 1] == 'S' && str[i + 2] == 'T') {
                                tokens.systemCommand.letter = 'R';
                                state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
                                i += 2;
                            }
                        }
                        else {
                            tokens = {};
                            return GrblError::UnsupportedOrInvalidGCodeCommand;
                        }
                    }
                    else {
                        tokens.systemCommand.letter = c;
                        state = InterpreterState::EXPECT_SYSTEM_CMD_INDEX;
                    }
                }
                else if (isDigit(c)) { // $ is followed by a number (Example $10=)
                    tokens.systemCommand.index = charToInt(c);
                    state = InterpreterState::EXPECT_SYSTEM_CMD_INDEX;
                }
                else if (c == ' ') { // Space between letter and number is not
                                     // allowed, we need '=' now
                    state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
                }
                else {
                    tokens = {};
                    return GrblError::UnsupportedOrInvalidGCodeCommand;
                }
                break;

            // ===============================================
            // EXPECT_SYSTEM_CMD_INDEX
            // ========================================================
            case InterpreterState::EXPECT_SYSTEM_CMD_INDEX:
                if (isDigit(c)) { // Enlarge the number (Example either '$N10=' or
                                  // '$10=')
                    tokens.systemCommand.index =
                        tokens.systemCommand.index * 10 + charToInt(c);
                }
                else if (c == '=') {
                    state = InterpreterState::EXPECT_SYSTEM_CMD_VALUE;
                    isNegative = false;
                    haveNumber = false;
                }
                else if (c == ' ') { // Space ends the index number
                    state = InterpreterState::EXPECT_SYSTEM_CMD_EQUALS;
                }
                else {
                    tokens = {};
                    return GrblError::UnsupportedOrInvalidGCodeCommand;
                }
                break;

            // ===============================================
            // EXPECT_SYSTEM_CMD_EQUALS
            // ========================================================
            case InterpreterState::EXPECT_SYSTEM_CMD_EQUALS:
                if (c == '=') {
                    state = InterpreterState::EXPECT_SYSTEM_CMD_VALUE;
                    isNegative = false;
                    haveNumber = false;
                }
                else if (c == ' ') { // Wait for '='
                                     // Do nothing
                }
                else {
                    tokens = {};
                    return GrblError::UnsupportedOrInvalidGCodeCommand;
                }
                break;

            // ===============================================
            // EXPECT_SYSTEM_CMD_VALUE
            // ========================================================
            case InterpreterState::EXPECT_SYSTEM_CMD_VALUE: // Now we can either
                                                            // find a number, or a
                                                            // full G-Code command
                if (inCapitalAlphabet(c) &&
                    !haveNumber) { // It is a G-Code command, continue
                                   // like a normal command
                    tokens.tokens[tokens.numberOfValidTokens].type = c;
                    state = InterpreterState::EXPECT_NUMBER;
                    isNegative = false;
                    haveNumber = false;
                }
                else if (c == ' ' && !haveNumber) { // Space after the '=' is allowed, not
                                                    // in the value Do nothing
                }
                else if (tokens.systemCommand.letter == 'R' &&
                         c == '$') { // Special case $RST=$
                    tokens.systemCommand.valueLetter = '$';
                }
                else if (tokens.systemCommand.letter == 'R' &&
                         c == '#') { // Special case $RST=#
                    tokens.systemCommand.valueLetter = '#';
                }
                else if (tokens.systemCommand.letter == 'R' &&
                         c == '*') { // Special case $RST=*
                    tokens.systemCommand.valueLetter = '*';
                }
                else if (isDigit(c)) { // It is a digit, append it to the value
                    tokens.systemCommand.value =
                        tokens.systemCommand.value * 10 + charToInt(c);
                    haveNumber = true;
                }
                else if (c == '.') { // Expect floating point digits now
                    state = InterpreterState::EXPECT_SYSTEM_CMD_VALUE_DECIMALS;
                    decimalPlace = 0.1f;
                }
                else if (c == '-') { // Mark to later negate the value
                    if (!haveNumber && !isNegative) {
                        isNegative = true;
                    }
                    else {
                        tokens = {};
                        return GrblError::GCodeCommandValueInvalidOrMissing;
                    }
                }
                break;

            // ===============================================
            // EXPECT_SYSTEM_CMD_VALUE_DECIMALS
            // ========================================================
            case InterpreterState::EXPECT_SYSTEM_CMD_VALUE_DECIMALS:
                if (isDigit(c)) {
                    tokens.systemCommand.value += charToInt(c) * decimalPlace;
                    decimalPlace *= 0.1f;
                    haveNumber = true;
                }
                else {
                    tokens = {};
                    return GrblError::UnsupportedOrInvalidGCodeCommand;
                }
                break;
        }
    }

    // Finish the state off
    switch (state) {
        case InterpreterState::EXPECT_LETTER: // This is fine
            break;

        case InterpreterState::EXPECT_NUMBER:   // This is fine if we have a number
        case InterpreterState::EXPECT_DECIMALS: // This is fine if we have a number
            if (haveNumber) {
                tokens.tokens[tokens.numberOfValidTokens].value *=
                    isNegative ? -1.f : 1.f;
                tokens.numberOfValidTokens++;
            }
            else {
                tokens = {};
                return GrblError::GCodeCommandValueInvalidOrMissing;
            }
            break;

        case InterpreterState::EXPECT_SYSTEM_CMD_LETTER: // This is never fine
            tokens = {};
            return GrblError::UnsupportedOrInvalidGCodeCommand;

        case InterpreterState::EXPECT_SYSTEM_CMD_INDEX:  // This is fine (Example:
                                                         // '$G')
        case InterpreterState::EXPECT_SYSTEM_CMD_EQUALS: // This is fine (Example:
                                                         // '$G10')
            break;

        case InterpreterState::EXPECT_SYSTEM_CMD_VALUE: // This is fine if we have a
                                                        // number
        case InterpreterState::EXPECT_SYSTEM_CMD_VALUE_DECIMALS: // This is fine if
                                                                 // we have a number
            if (haveNumber) {
                tokens.systemCommand.value *= isNegative ? -1.f : 1.f;
            }
            else {
                tokens = {};
                return GrblError::GCodeCommandValueInvalidOrMissing;
            }
            break;
        default:
            break;
    }

    return GrblError::None;
}

#endif // GCGP_TOKENIZEDCOMMAND_H
#endif // __cplusplus
