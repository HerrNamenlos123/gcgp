#ifdef __cplusplus
#ifndef GCGP_SETTINGS_H
#define GCGP_SETTINGS_H

#include "GCGP/Config.h"

class SettingsStorage {
public:
	SettingsStorage() {
		for (size_t i = 0; i < GCGP_MAX_NUMBER_OF_SETTINGS; i++) {
			m_values[i] = 0.f;
			memset(m_descriptions[i], 0, GCGP_MAX_SETTINGS_DESCRIPTION_LENGTH);
		}
	}

	float getValue(size_t index) {
		if (index >= GCGP_MAX_NUMBER_OF_SETTINGS) {
			return NAN;
		}
		return m_values[index];
	}

	const char* getDescription(size_t index) {
		if (index >= GCGP_MAX_NUMBER_OF_SETTINGS) {
			return nullptr;
		}
		return m_descriptions[index];
	}

	void setValue(size_t index, float value) {
		if (index >= GCGP_MAX_NUMBER_OF_SETTINGS) {
			return;
		}
		m_values[index] = value;
	}

	void setDescription(size_t index, const char* desc) {
		if (index >= GCGP_MAX_NUMBER_OF_SETTINGS) {
			return;
		}
		strncpy(m_descriptions[index], desc, GCGP_MAX_SETTINGS_DESCRIPTION_LENGTH);
	}

private:
	float m_values[GCGP_MAX_NUMBER_OF_SETTINGS];
	char m_descriptions[GCGP_MAX_SETTINGS_DESCRIPTION_LENGTH][GCGP_MAX_NUMBER_OF_SETTINGS];
};

#endif // GCGP_SETTINGS_H
#endif // __cplusplus