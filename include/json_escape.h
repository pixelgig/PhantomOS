#pragma once
#include <stddef.h>

namespace phantom {
constexpr size_t MAX_JSON_TEXT_BYTES = 32;
constexpr size_t QUOTED_TEXT_CAPACITY = MAX_JSON_TEXT_BYTES * 6 + 3;

// Preserve valid UTF-8. Represent invalid bytes and ASCII controls as
// individual JSON escapes. The caller supplies a bounded output buffer.
inline bool quoteJsonText(const char* input, size_t length, char (&output)[QUOTED_TEXT_CAPACITY]) {
  if (length > MAX_JSON_TEXT_BYTES) return false;
  const char hex[] = "0123456789abcdef";
  size_t written = 0;
  output[written++] = '"';
  for (size_t i = 0; i < length; ++i) {
    const unsigned char byte = input[i];
    if (byte == '"' || byte == '\\') {
      output[written++] = '\\';
      output[written++] = byte;
      continue;
    }
    if (byte >= 32 && byte < 127) {
      output[written++] = byte;
      continue;
    }
    if (byte >= 0xc2 && byte <= 0xf4) {
      const size_t width = byte < 0xe0 ? 2 : byte < 0xf0 ? 3 : 4;
      bool valid = width <= length - i;
      for (size_t j = 1; valid && j < width; ++j) {
        const unsigned char next = input[i + j];
        valid = next >= 0x80 && next <= 0xbf;
        if (j == 1 && ((byte == 0xe0 && next < 0xa0) || (byte == 0xed && next > 0x9f) ||
                       (byte == 0xf0 && next < 0x90) || (byte == 0xf4 && next > 0x8f))) {
          valid = false;
        }
      }
      if (valid) {
        for (size_t j = 0; j < width; ++j) output[written++] = input[i + j];
        i += width - 1;
        continue;
      }
    }
    output[written++] = '\\';
    output[written++] = 'u';
    output[written++] = '0';
    output[written++] = '0';
    output[written++] = hex[byte >> 4];
    output[written++] = hex[byte & 15];
  }
  output[written++] = '"';
  output[written] = '\0';
  return true;
}
}
