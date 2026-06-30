# LED POV Stick — ESP32-C3 + APA102

A persistence-of-vision (POV) display stick built with an **ESP32-C3** microcontroller, **72 APA102 (DotStar) LEDs**, and an **MPU6050 IMU** sensor. Swing the stick side-to-side and an image appears floating in the air.

> **POV (Persistence of Vision) 막대** — ESP32-C3와 72개의 APA102 LED, MPU6050 IMU 센서를 사용하여 막대를 흔들면 공중에 이미지를 띄웁니다.

---

## Features

- **72-LED column** — vertical resolution of the displayed image
- **Motion-synced rendering** — MPU6050 gyro detects zero-crossings to trigger each column refresh
- **Wi-Fi image upload** — browser-based drag-and-drop upload directly to the device (AP mode)
- **HSV color processing** — saturation boost (+50%) and brightness reduction (-30%) on the browser side for better POV appearance
- **Custom APA102 driver** — 20 MHz hardware SPI for fast LED data transfer (no FastLED library needed)
- **LittleFS storage** — uploaded image persists across reboots

---

## Hardware Requirements

| Component | Specification |
|---|---|
| MCU | ESP32-C3 (RISC-V, with Wi-Fi + BLE) |
| LEDs | APA102 (DotStar) × 72 |
| IMU | MPU6050 (6-axis accelerometer + gyroscope) |
| Status LED | Onboard (GPIO 8) |
| Switch | GPIO 9 (INPUT_PULLUP, future use) |

### Pin Connections

| ESP32-C3 Pin | Connected To |
|---|---|
| GPIO 6 | APA102 Data Input (DI) |
| GPIO 10 | APA102 Clock Input (CI) |
| GPIO 3 | MPU6050 SDA (I2C data) |
| GPIO 4 | MPU6050 SCL (I2C clock) |
| GPIO 0 | MPU6050 INT (interrupt output, INPUT_PULLDOWN, RISING edge) |
| GPIO 8 | Onboard LED (active high) |
| GPIO 9 | Push button switch (INPUT_PULLUP) |

> **Note:** APA102 runs at 5 V logic typical; ESP32-C3 is 3.3 V. A level shifter on the SPI lines (data & clock) is recommended if driving a long LED strip.

---

## How It Works

### 1. Image Upload
1. Power up the stick — it creates a Wi-Fi AP with SSID `POV_Stick_AP`.
2. Connect to that network and browse to `http://192.168.4.1`.
3. Drag & drop an image — the browser resizes it to **72 px tall** (max 300 px wide), processes colors in HSV space (saturation ×1.5, brightness ×0.7), and uploads the raw binary.
4. The ESP32 saves the image to LittleFS (`/image.raw`) and loads it into SRAM immediately.

### 2. Motion Detection
- The MPU6050 generates interrupts at **1 kHz** (data-ready).
- The firmware reads the **Z-axis gyroscope value (`gz`)** on every interrupt.
- A **zero-crossing** (sign change of `gz`) with a delta ≥ 400 triggers a swing event.
- Swing direction is determined: `gz < 0` → left-to-right, `gz > 0` → right-to-left.

### 3. POV Rendering
- Only the **middle 60 %** of each swing is used for display (first/last 20 % blanked to avoid cropping at turnaround points).
- Columns are rendered at intervals calculated from `swing_duration / image_width`.
- Direction compensation flips column order when swinging right-to-left so the image is not mirrored.
- After rendering, all LEDs are turned off.

---

## Software Configuration

| Define / Constant | Default Value | Description |
|---|---|---|
| `NUM_LEDS` | 72 | Number of APA102 LEDs (vertical resolution) |
| `DATA_PIN` | 6 | APA102 data pin |
| `CLK_PIN` | 10 | APA102 clock pin |
| `ONBOARD_LED_PIN` | 8 | Status LED pin |
| `SW_PIN` | 9 | User switch pin |
| `INTERRUPT_PIN` | 0 | MPU6050 interrupt pin |
| `IMAGE_FILE_PATH` | `"/image.raw"` | Image file on LittleFS |
| `ap_ssid` | `"POV_Stick_AP"` | Wi-Fi AP SSID |
| SPI frequency | 20 MHz | APA102 LED data rate |
| I2C frequency | 800 kHz | MPU6050 bus speed |
| `SATURATION_SCALE` | 1.5 | HSV saturation multiplier (browser-side) |
| `BRIGHTNESS_SCALE` | 0.7 | HSV brightness multiplier (browser-side) |

---

## Libraries Required

| Library | Purpose |
|---|---|
| `Wire.h` | Arduino I2C (MPU6050 communication) |
| `SPI.h` | Arduino SPI (APA102 driving) |
| `I2Cdev.h` | Generic I2C register access helper |
| `MPU6050.h` | MPU6050 sensor initialization & reading |
| `WiFi.h` | ESP32 Wi-Fi (AP mode) |
| `WebServer.h` | HTTP server for image upload |
| `LittleFS.h` | ESP32 LittleFS (file storage for images) |

---

## Building & Flashing

### Arduino IDE
1. Open `LED_POV_APA102.ino`.
2. Set board to **ESP32-C3 Dev Module**.
3. Install libraries: `MPU6050` by jrowberg, `I2Cdevlib-core` by jrowberg.
4. Ensure `webpage.h` is in the same folder as the `.ino` file.
5. Select a **partition scheme** with at least 4 MB flash (SPIFFS/LittleFS support required).
6. Compile and upload.

### PlatformIO
1. Create a new PlatformIO project for `ESP32-C3` with the Arduino framework.
2. Add to `lib_deps` in `platformio.ini`:
   ```ini
   lib_deps =
       jrowberg/I2Cdevlib-core
       jrowberg/MPU6050
   ```
3. Copy `LED_POV_APA102.ino` → `src/main.cpp`.
4. Copy `webpage.h` → `include/webpage.h`.
5. Build and upload.

### First Boot
- The stick creates the `POV_Stick_AP` Wi-Fi network.
- Connect to upload an image via the web interface at `http://192.168.4.1`.
- Once an image is uploaded, it is saved to LittleFS and loaded on every subsequent boot.

---

## File Structure

```
LED_POV_APA102/
├── LED_POV_APA102.ino    # Main firmware (setup, loop, motion detection, POV rendering,
│                         # APA102 SPI driver, web server, image upload handling)
├── webpage.h             # HTML page served by the web server (browser-side image
│                         # processing, resize, color transform, upload)
├── PLAN.md               # Hardware design notes and plan (Korean)
└── README.md             # This file
```

---

## Image Data Format (LittleFS)

The raw file stored on the device has a simple binary format:

1. **Header:** 2 bytes, big-endian image width (`img_width`, max 300).
2. **Pixel data:** `img_width × 72 × 3` bytes (R, G, B per LED per column).

Total size for a full-width image: 2 + (300 × 72 × 3) ≈ 64.8 KB.

---

## Browser-side Image Processing

When you upload an image via the web UI, JavaScript in the browser:

1. Resizes the image to **72 px tall** (width scales proportionally, capped at 300 px).
2. Converts RGB → HSV, adjusts saturation (×1.5) and value/brightness (×0.7), then converts back to RGB. This compensates for color washout typical in fast-scrolling POV displays.
3. Flips the image vertically (LED index 0 = bottom of the stick).
4. Sends the raw binary data as a `multipart/form-data` POST to `/upload`.

---

## Notes & Known Issues

- **Motion loop blocks during upload** — `is_uploading` flag prevents rendering while an image is being received, avoiding SRAM buffer corruption.
- **Idle timeout disabled** — auto-turn-off logic is commented out; LEDs stay on the last frame.
- **Interrupt-based early abort disabled** — once `display_pov()` starts, it runs to completion without interruption.
- **MPU6050 failure = hard lock** — if the IMU is not detected during `setup()`, the board blinks the onboard LED every 100 ms in an infinite loop.
- **No FastLED** — APA102 is driven directly via 20 MHz hardware SPI for maximum throughput. A custom `CRGB` struct provides basic color assignment.
- **End frame calculation** — APA102 end frame length is `(NUM_LEDS + 15) / 16` bytes, matching the protocol specification.

---

## License

This project is shared for educational and hobbyist use.
