#ifdef __cplusplus
#ifndef GCGP_CONFIG_H
#define GCGP_CONFIG_H

#ifdef ARDUINO
#include <Arduino.h>
#else
#include <string.h>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <cinttypes>
#endif

#ifndef GCGP_MAX_COMMAND_LENGTH
#define GCGP_MAX_COMMAND_LENGTH 80
#endif

#ifndef GCGP_MAX_NUM_OF_CMD_TOKENS
#define GCGP_MAX_NUM_OF_CMD_TOKENS 10
#endif

#ifndef GCGP_MAX_NUMBER_OF_SETTINGS
#define GCGP_MAX_NUMBER_OF_SETTINGS 16
#endif

#ifndef GCGP_MAX_SETTINGS_DESCRIPTION_LENGTH
#define GCGP_MAX_SETTINGS_DESCRIPTION_LENGTH 32
#endif

#define GCGP_WELCOME_MESSAGE "GCGP v0.1 - pretending to be Grbl 1.1h [$ help]"
#define GCGP_OK_MESSAGE "ok"
#define GCGP_ERROR_MESSAGE "error:"

#define GCGP_CMD_CYCLE_START '~'
#define GCGP_CMD_FEED_HOLD '!'
#define GCGP_CMD_SOFT_RESET '\x18'
#define GCGP_CMD_STATUS_REPORT '?'
#define GCGP_SYSTEM_CMD '$'

#endif // GCGP_CONFIG_H
#endif // __cplusplus
