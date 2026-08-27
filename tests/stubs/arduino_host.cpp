/**
 * @file arduino_host.cpp
 * @copyright Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 *            Muhammad Samiullah — CTO & Founder. All rights reserved.
 */
#include "Arduino.h"

HardwareSerial Serial;

static unsigned long s_ms;
static uint8_t s_gpio[64];

unsigned long millis(void)
{
    return s_ms++;
}

unsigned long micros(void)
{
    return s_ms * 1000ul;
}

void delay(unsigned long ms)
{
    s_ms += ms;
}

void pinMode(uint8_t pin, uint8_t mode)
{
    (void)pin;
    (void)mode;
}

void digitalWrite(uint8_t pin, uint8_t val)
{
    if (pin < 64u) {
        s_gpio[pin] = (uint8_t)(val != 0);
    }
}

int digitalRead(uint8_t pin)
{
    return (pin < 64u) ? (int)s_gpio[pin] : 0;
}

int analogRead(uint8_t pin)
{
    (void)pin;
    return 2048;
}

void HardwareSerial::begin(unsigned long baud)
{
    (void)baud;
}

size_t HardwareSerial::write(const uint8_t *buf, size_t n)
{
    (void)buf;
    return n;
}
