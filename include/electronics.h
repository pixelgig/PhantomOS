#pragma once
#include <Arduino.h>

namespace electronics {
struct Pin {
  const char* label;
  uint8_t gpio;
};

// Only general-purpose pins on the pinned NodeMCU variant.
constexpr Pin INPUTS[] = {{"D1", D1}, {"D2", D2}, {"D5", D5}, {"D6", D6}, {"D7", D7}};
String readInputs();
}
