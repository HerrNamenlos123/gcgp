#ifdef __cplusplus
#ifndef GCGP_STRING_H
#define GCGP_STRING_H

#include "GCGP/Config.h"

class StringView {
public:
    StringView() = default;
    StringView(char* data, size_t length) : m_data(data), m_length(length) {}
    StringView(char* data) : m_data(data), m_length(strlen(data)) {}

    char* data() const { return m_data; }
    size_t length() const { return m_length; }

    char& operator[](size_t index) { return m_data[index]; }
    const char& operator[](size_t index) const { return m_data[index]; }

private:
    char* m_data = nullptr;
    size_t m_length = 0;
};

#endif // GCGP_STRING_H
#endif // __cplusplus