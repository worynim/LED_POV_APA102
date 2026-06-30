# Release Notes

## v2.1 (2026-06-30)

### ✨ New Features

- **Thumbnail preview in web UI** — saved images now show a 72 px pixel‑accurate thumbnail next to the file name
- **Current image info display** — web UI clearly shows which image is currently active, with name and position (e.g. "cat.png (3/5)")
- **Image name input** — file name can be edited before upload; auto‑filled from original file name, useful when mobile browsers assign temporary numeric names
- **Web image selection** — click "선택" on any saved image to switch to it remotely
- **Web image deletion** — click "삭제" to remove a saved image; automatically switches to the next available image if the current one is deleted
- **`GET /data?index=N` API** — serves raw image binary data for client‑side thumbnail rendering

### 🐛 Bug Fixes

- **Upload file name preservation** — file name is now sent as a query parameter (`POST /upload?name=...`) instead of relying solely on multipart headers, fixing Korean and special‑character file names
- **`</script>` tag missing** — the HTML page was missing its closing `</script>` tag, which prevented JavaScript from executing properly
- **Button LED indicator shows residual image data** — `indicate_image_index()` now clears the entire `leds[]` buffer before lighting up the indicator LEDs
- **Button debounce logic** — fixed a race condition where `last_button_reading` was overwritten before the debounce timer expired, making button presses undetected
- **Upload handler temp‑file bug** — removed dependency on a non‑existent `/tmp/` directory; images are now written directly to `/img/N.raw`
- **`server.uri()` query string parsing** — replaced manual URI parsing with `server.arg("name")` which properly handles URL‑decoded query parameters

---

## v2.0 (2026-06-30)

### ✨ New Features

- **Multi‑image storage** — up to 10 images stored on LittleFS (`/img/0.raw` … `/img/9.raw`)
- **Button image cycling (GPIO9)** — press the button to cycle through saved images; LED bar lights up `(index + 1)` LEDs for visual feedback
- **Web image list** — web page now displays all saved images with select and delete buttons
- **Current image indicator** — the active image is highlighted in the web UI and shown in a dedicated "현재 디스플레이" section
- **`GET /list` API** — returns JSON list of saved images and current index
- **`POST /select` API** — switch to a specific image by index
- **`POST /delete` API** — delete a specific image by index

### 📦 Image Storage Structure

```
Old: /image.raw                     (single image)
New: /img/0.raw … /img/9.raw       (up to 10 images)
     /img/info.txt                  (metadata: current index, file names)
```

### ⚠️ Breaking Changes

- The image storage format changed from a single `/image.raw` file to a multi‑file layout in `/img/`. **Existing `/image.raw` files are not migrated.** Upload your images again after flashing.

---

## v1.0 (Initial Release)

### Features

- ESP32‑C3 based POV display stick
- 72 × APA102 LED column
- MPU6050 gyroscope motion detection
- 20 MHz hardware SPI APA102 driver
- Wi‑Fi AP mode (`POV_Stick_AP`) with web‑based image upload
- HSV color processing (saturation ×1.5, brightness ×0.7)
- LittleFS persistent storage
- Single image storage (`/image.raw`)
