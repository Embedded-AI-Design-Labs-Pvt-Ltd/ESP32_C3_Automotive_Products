/**
 * @file Arduino.h
 * @brief Host stub of the Arduino core so hal_arduino.cpp can be unit-tested.
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#ifndef ARDUINO_STUB_H
#define ARDUINO_STUB_H

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define HIGH 1
#define LOW 0
#define INPUT 0
#define OUTPUT 1

unsigned long millis(void);
unsigned long micros(void);
void delay(unsigned long ms);
void pinMode(uint8_t pin, uint8_t mode);
void digitalWrite(uint8_t pin, uint8_t val);
int digitalRead(uint8_t pin);
int analogRead(uint8_t pin);

class HardwareSerial {
public:
    void begin(unsigned long baud);
    size_t write(const uint8_t *buf, size_t n);
};

extern HardwareSerial Serial;

#endif /* ARDUINO_STUB_H */
