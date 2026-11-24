#include <cassert>
#include <iostream>
#include <string>
#include "json_escape.h"

std::string quote(const std::string& input) {
  char output[phantom::QUOTED_TEXT_CAPACITY];
  assert(phantom::quoteJsonText(input.data(), input.size(), output));
  return output;
}

int main() {
  assert(quote("") == R"("")");
  assert(quote("a\"b\\c") == R"("a\"b\\c")");
  assert(quote(std::string("\0\t\x7f", 3)) == R"("\u0000\u0009\u007f")");
  assert(quote(u8"हिंदी café 📶") == u8"\"हिंदी café 📶\"");
  assert(quote("\xc0\xaf\xed\xa0\x80\xf4\x90\x80\x80\xe2") ==
         R"("\u00c0\u00af\u00ed\u00a0\u0080\u00f4\u0090\u0080\u0080\u00e2")");

  const std::string worst(phantom::MAX_JSON_TEXT_BYTES, '\xff');
  std::string expected = "\"";
  for (size_t i = 0; i < worst.size(); ++i) expected += "\\u00ff";
  expected += '"';
  assert(quote(worst) == expected);
  char output[phantom::QUOTED_TEXT_CAPACITY];
  const std::string tooLong(phantom::MAX_JSON_TEXT_BYTES + 1, 'a');
  assert(!phantom::quoteJsonText(tooLong.data(), tooLong.size(), output));
  std::cout << "PASS: JSON encoding, malformed UTF-8 and maximum expansion\n";
}
