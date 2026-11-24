#include "electronics.h"

namespace electronics {
String readInputs() {
  String response;
  if (!response.reserve(256)) return {};
  response += F("{\"schema\":1,\"pins\":[");
  bool first = true;
  for (const Pin& pin : INPUTS) {
    if (!first) response += ',';
    first = false;
    response += F("{\"label\":\"");
    response += pin.label;
    response += F("\",\"gpio\":");
    response += pin.gpio;
    response += F(",\"level\":");
    response += digitalRead(pin.gpio) == HIGH ? '1' : '0';
    response += '}';
  }
  response += F("]}");
  return response;
}
}
