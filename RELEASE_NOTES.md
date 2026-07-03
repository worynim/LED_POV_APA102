# Release Notes

## v2.3 (2026-07-03)

### ✨ New Features

- **Text mode** — type text directly in the web UI and display on the POV stick without needing an image file
  - Text input with live canvas preview
  - 10-system-font dropdown + custom font file loader (.ttf, .otf, .woff, .woff2)
  - Font size slider (16–72 px), color picker
  - Multi-line text support (Enter for new line)
  - Auto-crop: pixel-scan detects actual text bounds, aligns text exactly to top of display
  - Character counter: Korean/CJK = 2 slots, ASCII = 1 slot, max 40 slots (Korean 20 / English 40)
- **Custom font loading** — load .ttf/.otf/.woff/.woff2 font files from your computer via FontFace API
- **Text mode raw color output** — text renders with exact selected color (no HSV saturation/brightness filter; image mode filter unchanged)
- **Increased storage** — `MAX_IMAGES` raised from 10 → 20 slots
- **Mode tabs** — image/text mode switcher in web UI

### 🔧 Improvements

- **Font dropdown** — replaced single-line input with proper `<select>` dropdown (10 system fonts)
- **Text mode preview** — 150 ms debounced rendering for smooth typing experience
- **Upload validation** — text mode blocks upload if empty or over character limit

---

## v2.2 (2026-06-30)

### ✨ New Features

- **Mid‑swing direction abort** — `display_pov()` checks MPU6050 gyro after every column; if swing direction reverses mid‑render, LEDs turn off immediately instead of showing a garbled trailing image

### 🐛 Bug Fixes

- **`display_pov()` first‑column false abort** — `prev_gz_val` now synced to current gyro value at render start; previously held the pre‑zero‑crossing (opposite‑direction) value, causing an instant abort on the first column
- **`load_image_to_sram()` buffer safety** — old image buffer is now freed only after the new file is successfully read; a missing or corrupt file no longer wipes the currently displayed image

### 🔧 Improvements

- **Code cleanup** — removed unused globals (`last_swing_time`, `IDLE_TIMEOUT`), dead commented‑out code blocks, and stale comments referencing FastLED
- **`clear_apa102_fast()`** — LED off value changed from `0xE0000000` to `0xFF000000` for clearer intent (APA102 brightness = 0)
- **`handle_list_images()` heap safety** — JSON string pre‑allocated with `reserve(512)` to reduce heap fragmentation
- **Offline web UI** — removed Google Fonts dependency (`Outfit` replaced with `system-ui`); page renders instantly without internet access
- **Debug log** — `display_pov()` runtime log disabled by default (`#if 0`); enable for debugging

---

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
