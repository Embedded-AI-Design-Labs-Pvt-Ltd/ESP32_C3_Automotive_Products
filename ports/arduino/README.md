# Arduino IDE (ESP32-C3)

The sketch `AEGW_C3` is self-contained: `src/` holds copies of the platform C sources plus `hal_arduino.cpp` (TWAI GPIO4 TX / GPIO5 RX at 500 kbit/s, software bus fallback).

1. Install **Arduino IDE 2.x** and the **esp32** board package (ESP32C3 Dev Module).
2. File → Open `ports/arduino/AEGW_C3/AEGW_C3.ino`.
3. Tools → Board → ESP32C3 Dev Module. Enable **USB CDC On Boot**.
4. Tools → Port → the ESP32-C3 COM port (not Intel AMT SOL).
5. Sketch → Upload.

Without a CAN transceiver the firmware still runs all 17 products on the software bus.

Serial 115200: `0`-`9` / `a`-`g` select P01–P17, `r` recover, `?` help.

Regenerate `src/` copies after platform file changes (does not overwrite `hal_arduino.cpp`):

```
python tools/scripts/gen_arduino_sketch.py
```

Command-line build (Arduino CLI 1.5+ with esp32:esp32 3.x):

```
arduino-cli compile --fqbn esp32:esp32:esp32c3:CDCOnBoot=cdc ports/arduino/AEGW_C3
arduino-cli upload -p COMx --fqbn esp32:esp32:esp32c3:CDCOnBoot=cdc ports/arduino/AEGW_C3
```

---

**Embedded AI Design Labs Pvt Ltd**  
Muhammad Samiullah  
CTO & Founder  
© 2026 Copyright. All rights reserved.

