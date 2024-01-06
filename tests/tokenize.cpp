
#include <GCGP/tokenize.h>
#include <catch2/catch_test_macros.hpp>
#include <iostream>
#include <string>
#include <vector>

std::vector<std::tuple<std::string, std::string, CommandTokens<10>, GrblError>> testdata =
    {
        {"Empty command",
         "",
         {.tokens = {},
          .numberOfValidTokens = 0,
          .isSystemCommand = false,
          .systemCommand =
              {.letter = '\0', .index = -1, .value = 0.f, .valueLetter = '\0'}},
         GrblError::None},
        {"Very simple command",
         "G1",
         {.tokens = {{'G', 1.f}},
          .numberOfValidTokens = 1,
          .isSystemCommand = false,
          .systemCommand =
              {
                  .letter = '\0',
                  .index = -1,
                  .value = 0.f,
                  .valueLetter = '\0',
              }},
         GrblError::None},
        {
            "Longest 10-parameter command with worst formatting",
            "G0X17   F17.2G39.12F-1. J5 X-.2     Z-38.K00001 X-0.",
            {.tokens = {{'G', 0.f},
                        {'X', 17.f},
                        {'F', 17.2f},
                        {'G', 39.12f},
                        {'F', -1.f},
                        {'J', 5.f},
                        {'X', -0.2f},
                        {'Z', -38.f},
                        {'K', 1.f},
                        {'X', 0.f}},
             .numberOfValidTokens = 10,
             .isSystemCommand = false,
             .systemCommand =
                 {
                     .letter = '\0',
                     .index = -1,
                     .value = 0.f,
                     .valueLetter = '\0',
                 }},
            GrblError::None,
        },
        {
            "Testing EXPECT_LETTER: Leading spaces",
            "  G0X3       Z17    Y3",
            {.tokens = {{'G', 0.f}, {'X', 3.f}, {'Z', 17.f}, {'Y', 3.f}},
             .numberOfValidTokens = 4,
             .isSystemCommand = false,
             .systemCommand =
                 {
                     .letter = '\0',
                     .index = -1,
                     .value = 0.f,
                     .valueLetter = '\0',
                 }},
            GrblError::None,
        },
        {
            "Testing EXPECT_LETTER: Wrong commands",
            "  G4 Ü2",
            {.tokens = {},
             .numberOfValidTokens = 0,
             .isSystemCommand = false,
             .systemCommand =
                 {
                     .letter = '\0',
                     .index = -1,
                     .value = 0.f,
                     .valueLetter = '\0',
                 }},
            GrblError::UnsupportedOrInvalidGCodeCommand,
        },
        {
            "Testing EXPECT_NUMBER: Digits, dots, minus, Letter, space",
            "G0 G.0 G-2 G1G2 G2 ",
            {.tokens =
                 {{'G', 0.f}, {'G', 0.f}, {'G', -2.f}, {'G', 1}, {'G', 2.f}, {'G', 2.f}},
             .numberOfValidTokens = 6,
             .isSystemCommand = false,
             .systemCommand =
                 {
                     .letter = '\0',
                     .index = -1,
                     .value = 0.f,
                     .valueLetter = '\0',
                 }},
            GrblError::None,
        },
        {
            "Too many parameters",
            "G0G1G2G3G4G5G6G7G8G9G10",
            {.tokens = {},
             .numberOfValidTokens = 0,
             .isSystemCommand = false,
             .systemCommand =
                 {
                     .letter = '\0',
                     .index = -1,
                     .value = 0.f,
                     .valueLetter = '\0',
                 }},
            GrblError::GCodeTooManyParameters,
        },
};

std::string printchar(char c)
{
    if (c >= 32 && c <= 126) {
        return std::string(&c, 1);
    }
    else {
        return "'\\x" + std::to_string((int)c) + "'";
    }
}

TEST_CASE("tokenize", "tokenize")
{
    for (auto [comment, command, result, resultCode] : testdata) {
        std::cout << "Comment: " << comment << std::endl;
        CommandTokens<10> tokens;
        GrblError errorCode = tokenizeCommand(tokens, command.c_str(), command.length());
        REQUIRE(errorCode == resultCode);
        if (!(tokens == result)) {
            std::cout << "Command: " << command << std::endl;
            std::cout << "result.numberOfValidTokens = " << result.numberOfValidTokens;
            std::cout << ", but received " << tokens.numberOfValidTokens << std::endl;

            std::cout << "result.isSystemCommand = " << result.isSystemCommand;
            std::cout << ", but received " << tokens.isSystemCommand << std::endl;

            std::cout << "result.systemCommand.letter = "
                      << printchar(result.systemCommand.letter);
            std::cout << ", but received " << printchar(tokens.systemCommand.letter)
                      << std::endl;

            std::cout << "result.systemCommand.value = " << result.systemCommand.value;
            std::cout << ", but received " << tokens.systemCommand.value << std::endl;

            std::cout << "result.systemCommand.index = " << result.systemCommand.index;
            std::cout << ", but received " << tokens.systemCommand.index << std::endl;

            std::cout << "result.systemCommand.valueLetter = "
                      << printchar(result.systemCommand.valueLetter);
            std::cout << ", but received " << printchar(tokens.systemCommand.valueLetter)
                      << std::endl;

            for (int i = 0; i < 10; i++) {
                std::cout << "result.tokens[" << i << "] = <"
                          << printchar(result.tokens[i].type) << ","
                          << result.tokens[i].value << ">\n";
            }
            std::cout << std::endl;

            REQUIRE(tokens == result);
        }
    }
}
