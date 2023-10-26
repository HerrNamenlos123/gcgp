#ifdef __cplusplus
#ifndef GCGP_TOKENIZEDCOMMAND_H
#define GCGP_TOKENIZEDCOMMAND_H

#include "GCGP/Config.h"
#include "GCGP/Enums.h"
#include "GCGP/String.h"

class TokenizedCommand {
public:
	TokenizedCommand();
	TokenizedCommand(const StringView& str);

	bool getString(StringView& str) const;

protected:
	bool tokenize(const StringView& str);
    bool isSystemCommand() const;
	void clear();

    GrblError m_errorCode = GrblError::None;

	char m_tokenType[GCGP_MAX_NUM_OF_CMD_TOKENS];
	float m_tokenValue[GCGP_MAX_NUM_OF_CMD_TOKENS];
	size_t m_numTokens = 0;
	bool m_isValid = false;

    char m_systemCommandLetter = 0;
    int m_systemCommandIndex = -1;
    float m_systemCommandValue = NAN;
    char m_systemCommandValueLetter = 0;
};

#endif // GCGP_TOKENIZEDCOMMAND_H
#endif // __cplusplus