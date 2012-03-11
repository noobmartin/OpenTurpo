#include "logger.h"

#include <Arduino.h>

void logger::logMessage(const char *message) {
	Serial.print("Time: ");
	Serial.println(message);
}
