#include "logger.h"

#include <Arduino.h>

logger::logger() {
	Serial.begin(115200);
}

logger::~logger() {

}


void logger::logMessage(const char *message) {
	Serial.print("[Uptime ");
	Serial.print(micros() * MICROS_TO_SECONDS);
	Serial.print(" seconds]: ");
	Serial.println(message);
}

void logger::logMessage(const char *message, float value) {
	Serial.print("[Uptime ");
	Serial.print(micros() * MICROS_TO_SECONDS);
	Serial.print(" seconds]: ");
	Serial.print(message);
	Serial.print(": ");
	Serial.println(value);
}
void logger::logMessage(const char *message, int value) {
	Serial.print("[Uptime ");
	Serial.print(micros() * MICROS_TO_SECONDS);
	Serial.print(" seconds]: ");
	Serial.print(message);
	Serial.print(": ");
	Serial.println(value);
}

void logger::logMessage(const char *message, unsigned int value) {
	Serial.print("[Uptime ");
	Serial.print(micros() * MICROS_TO_SECONDS);
	Serial.print(" seconds]: ");
	Serial.print(message);
	Serial.print(": ");
	Serial.println(value);
}

void logger::logMessage(const char* message, byte value) {
  	Serial.print("[Uptime ");
	Serial.print(micros() * MICROS_TO_SECONDS);
	Serial.print(" seconds]: ");
	Serial.print(message);
	Serial.print(": ");
	Serial.println(value);
}
