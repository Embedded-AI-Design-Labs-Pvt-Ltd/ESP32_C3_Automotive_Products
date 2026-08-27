/*
 * AEGW_C3.ino — Arduino IDE sketch for ESP32-C3
 *
 * Board: ESP32C3 Dev Module
 * USB CDC On Boot: Enabled
 * Open this file in Arduino IDE, then Sketch → Upload.
 *
 * Serial 115200: keys 0-9,a-g select products P01-P17; 'r' recover; '?' help.
 *
 * Copyright (c) 2026 Embedded AI Design Labs Pvt Ltd.
 * Muhammad Samiullah — CTO & Founder.
 * © 2026 Copyright. All rights reserved.
 */

#include "src/aegw_runtime.h"
#include "src/fault_mgr.h"
#include "src/hal_misc.h"
#include "src/product_api.h"

void setup()
{
  Serial.begin(115200);
  delay(300);
  aegw_runtime_setup();
  Serial.println();
  Serial.println(F("AEGW-C3 ESP32-C3 runtime"));
  Serial.println(F("Embedded AI Design Labs"));
  Serial.println(F("Embedded AI Design Labs Pvt Ltd"));
  Serial.println(F("Muhammad Samiullah"));
  Serial.println(F("CTO & Founder"));
  Serial.println(F("© 2026 Copyright. All rights reserved."));
  Serial.println(F("keys 0-9,a-g = P01-P17  r=recover  ?=help"));
}

void loop()
{
  uint8_t n = 0;
  const ae_product_desc_t *tab = ae_products_table(&n);

  while (Serial.available() > 0) {
    int ch = Serial.read();
    if (ch == '?') {
      uint8_t i;
      for (i = 0; i < n; i++) {
        Serial.print(tab[i].id);
        Serial.print(' ');
        Serial.println(tab[i].name);
      }
    } else if (ch == 'r') {
      (void)fault_recover();
      Serial.println(F("NORMAL"));
    } else {
      uint8_t idx = 255;
      if (ch >= '0' && ch <= '9') {
        idx = (uint8_t)(ch - '0');
      } else if (ch >= 'a' && ch <= 'g') {
        idx = (uint8_t)(10 + (ch - 'a'));
      }
      if (idx < n) {
        if (aegw_runtime_select(idx) == 0) {
          Serial.print(F("selected "));
          Serial.println(tab[idx].id);
        }
      }
    }
  }
  aegw_runtime_loop();
}
