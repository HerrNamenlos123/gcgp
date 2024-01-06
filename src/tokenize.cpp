
#include "GCGP/tokenize.h"

//
// bool getString(char *str, size_t length) const
// {
//     if (!m_isValid) {
//         return false;
//     }
//
//     size_t index = 0;
//     if (index >= length) {
//         return false;
//     }
//     if (isSystemCommand()) {
//         str[index++] = '$';
//         if (index >= length) {
//             return false;
//         }
//         if (m_systemCommandLetter != 0) {
//             str[index++] = m_systemCommandLetter;
//             if (index >= length) {
//                 return false;
//             }
//         }
//         if (m_systemCommandIndex != -1) {
//             str[index++] = m_systemCommandIndex + '0';
//             if (index >= length) {
//                 return false;
//             }
//         }
//         if (m_systemCommandValueLetter != 0 || !isnan(m_systemCommandValue)) {
//             str[index++] = '=';
//             if (index >= length) {
//                 return false;
//             }
//             if (m_systemCommandValueLetter != 0) {
//                 str[index++] = m_systemCommandValueLetter;
//                 if (index >= length) {
//                     return false;
//                 }
//             }
//             if (!isnan(m_systemCommandValue)) {
//                 index += snprintf(
//                     str + index, length - index, "%d.%06d",
//                     static_cast<uint32_t>(floor((double)m_systemCommandValue)),
//                     static_cast<uint32_t>(floor((double)m_systemCommandValue * 1000000)) %
//                         1000000);
//                 if (index >= length) {
//                     return false;
//                 }
//             }
//         }
//     }
//     for (size_t i = 0; i < m_numTokens; i++) {
//         str[index++] = m_tokenType[i];
//         if (index >= length) {
//             return false;
//         }
//         index += snprintf(
//             str + index, length - index, "%d.%06d",
//             static_cast<uint32_t>(floor((double)m_tokenValue[i])),
//             static_cast<uint32_t>(floor((double)m_tokenValue[i] * 1000000)) % 1000000);
//         if (index >= length) {
//             return false;
//         }
//         if (i < m_numTokens - 1) {
//             str[index++] = ' ';
//             if (index >= length) {
//                 return false;
//             }
//         }
//     }
//     str[index] = '\0';
//     return true;
// }



