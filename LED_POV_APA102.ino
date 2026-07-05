#include <Wire.h>
#include <SPI.h>
#include "I2Cdev.h"
#include "MPU6050.h"
#include <WiFi.h>
#include <WebServer.h>
#include <LittleFS.h>
#include "webpage.h"

// --- 핀 및 하드웨어 설정 ---
#define NUM_LEDS 72
#define DATA_PIN 6
#define CLK_PIN 10
#define ONBOARD_LED_PIN 8
#define SW_PIN 9
#define INTERRUPT_PIN 0  // MPU6050 INT 핀을 ESP32C3 GPIO 0에 연결

struct CRGB {
  uint8_t r;  // 빨간색 (0 ~ 255)
  uint8_t g;  // 초록색 (0 ~ 255)
  uint8_t b;  // 파란색 (0 ~ 255)
};


CRGB leds[NUM_LEDS];
MPU6050 mpu;

// --- Wi-Fi 및 웹서버 설정 ---
const char* ap_ssid = "POV_Stick_AP";
WebServer server(80);


// --- 다중 이미지 설정 ---
#define MAX_IMAGES 20
#define IMG_DIR "/img"
#define IMG_INFO_FILE "/img/info.txt"
#define MAX_IMG_NAME_LEN 24

uint8_t image_count = 0;
uint8_t current_image_index = 0;
char image_names[MAX_IMAGES][MAX_IMG_NAME_LEN + 1] = {0};
bool image_slots_used[MAX_IMAGES] = {false};

// --- 이미지 SRAM 버퍼 ---
uint16_t img_width = 0;
uint8_t* img_buffer = nullptr;
String get_image_path(uint8_t index) {
  String path = IMG_DIR;
  path += "/";
  path += index;
  path += ".raw";
  return path;
}

// --- MPU6050 인터럽트 및 모션 변수 ---
volatile bool MPUInterrupt = false;
void IRAM_ATTR mpuInterruptHandler() {
  MPUInterrupt = true;
}

// 스윙 감지 변수
int16_t gz_val = 0;
int16_t prev_gz_val = 0;
unsigned long last_zero_crossing_time = 0;
unsigned long swing_duration = 200;  // 기본 스윙 시간 (ms)
bool swing_direction = false;        // true: 좌->우, false: 우->좌
#define SWING_THRESHOLD 400          // 스윙 방향 전환 감지 임계값

// --- 이미지 업로드 임시 파일 포인터 및 상태 플래그 ---
File* uploadFile = nullptr;
volatile bool is_uploading = false;

// --- 버튼 상태 변수 ---
unsigned long last_button_debounce_time = 0;
const unsigned long DEBOUNCE_DELAY = 50;
bool last_button_reading = HIGH;
bool button_state = HIGH;   // 디바운스된 안정 상태
bool button_pressed = false;

// --- LittleFS 유틸리티 함수 ---
void ensure_img_dir() {
  if (!LittleFS.exists(IMG_DIR)) {
    LittleFS.mkdir(IMG_DIR);
    Serial.println("[LittleFS] /img 디렉토리 생성됨");
  }
}

void save_image_info() {
  File file = LittleFS.open(IMG_INFO_FILE, "w");
  if (!file) {
    Serial.println("[LittleFS] info.txt 쓰기 실패");
    return;
  }
  file.printf("current=%d\n", current_image_index);
  file.printf("count=%d\n", image_count);
  for (int i = 0; i < MAX_IMAGES; i++) {
    if (image_slots_used[i]) {
      file.printf("%d=%s\n", i, image_names[i]);
    }
  }
  file.close();
}

void load_image_info() {
  if (!LittleFS.exists(IMG_INFO_FILE)) {
    // 최초 부팅: 기본값으로 info.txt 생성
    image_count = 0;
    current_image_index = 0;
    for (int i = 0; i < MAX_IMAGES; i++) {
      image_slots_used[i] = false;
      image_names[i][0] = '\0';
    }
    save_image_info();
    return;
  }

  File file = LittleFS.open(IMG_INFO_FILE, "r");
  if (!file) return;

  for (int i = 0; i < MAX_IMAGES; i++) {
    image_slots_used[i] = false;
    image_names[i][0] = '\0';
  }
  image_count = 0;
  current_image_index = 0;

  while (file.available()) {
    String line = file.readStringUntil('\n');
    line.trim();
    if (line.length() == 0) continue;

    if (line.startsWith("current=")) {
      current_image_index = line.substring(8).toInt();
      if (current_image_index >= MAX_IMAGES) current_image_index = 0;
    } else if (line.startsWith("count=")) {
      image_count = line.substring(6).toInt();
    } else {
      int eq = line.indexOf('=');
      if (eq > 0) {
        int idx = line.substring(0, eq).toInt();
        if (idx >= 0 && idx < MAX_IMAGES) {
          image_slots_used[idx] = true;
          String name = line.substring(eq + 1);
          name.toCharArray(image_names[idx], MAX_IMG_NAME_LEN + 1);
        }
      }
    }
  }
  file.close();
}

void delete_image(uint8_t index) {
  if (index >= MAX_IMAGES || !image_slots_used[index]) return;

  String path = get_image_path(index);
  LittleFS.remove(path);
  image_slots_used[index] = false;
  image_names[index][0] = '\0';
  image_count--;

  save_image_info();
  Serial.printf("[LittleFS] 이미지 %d 삭제됨 (남은 이미지: %d)\n", index, image_count);
}

// --- LittleFS에서 이미지를 SRAM으로 적재하는 함수 ---
void load_image_to_sram(uint8_t index) {
  if (!image_slots_used[index]) {
    Serial.printf("[SRAM] 이미지 %d: 슬롯이 비어 있습니다.\n", index);
    return;
  }

  String path = get_image_path(index);
  if (!LittleFS.exists(path)) {
    Serial.printf("[LittleFS] 이미지 파일 없음: %s\n", path.c_str());
    image_slots_used[index] = false;
    image_names[index][0] = '\0';
    image_count--;
    save_image_info();
    return;
  }

  File file = LittleFS.open(path, "r");
  if (!file) {
    Serial.printf("[LittleFS] 이미지 파일 열기 실패: %s\n", path.c_str());
    return;
  }

  // 1. 가로 해상도(Width) 읽기 (2바이트, Big Endian)
  uint16_t new_width = 0;
  if (file.available() >= 2) {
    uint8_t h_byte = file.read();
    uint8_t l_byte = file.read();
    new_width = (h_byte << 8) | l_byte;
  }

  if (new_width == 0 || new_width > 300) {
    Serial.printf("[LittleFS] 유효하지 않은 가로 크기: %d px\n", new_width);
    file.close();
    return;
  }

  // 2. 새 RGB 데이터 영역 메모리 할당
  size_t data_size = new_width * NUM_LEDS * 3;
  uint8_t* new_buffer = (uint8_t*)calloc(1, data_size);
  if (new_buffer == nullptr) {
    Serial.println("[Memory] SRAM 메모리 할당 실패!");
    file.close();
    return;
  }

  // 3. 파일에서 데이터 읽기
  size_t read_bytes = file.read(new_buffer, data_size);
  file.close();

  // 4. 새 데이터가 확보된 후에만 기존 버퍼 교체
  if (img_buffer != nullptr) {
    free(img_buffer);
  }
  img_buffer = new_buffer;
  img_width = new_width;

  Serial.printf("[LittleFS] 이미지 %d 로드 성공: %d x 72 px (%d bytes 읽음) [%s]\n",
    index, img_width, read_bytes, image_names[index]);
}

// --- HTTP POST 업로드 핸들러 (다중 슬롯 지원, 직접 쓰기) ---
String upload_filename = "";
int upload_slot = -1;
void handle_image_upload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("[Upload] 이미지 업로드 시작...");
    is_uploading = true;

    // 파일명: server.arg()로 query string에서 직접 가져옴
    Serial.printf("[Upload] args=%d, arg('name')='%s', upload.filename='%s'\n",
      server.args(), server.arg("name").c_str(), upload.filename.c_str());
    upload_filename = server.arg("name");
    if (upload_filename.length() == 0) {
      // fallback: multipart filename에서 추출
      String disp = upload.filename;
      if (disp.length() > 0) {
        int lastSlash = disp.lastIndexOf('/');
        int lastBackslash = disp.lastIndexOf('\\');
        int start = (lastSlash > lastBackslash) ? lastSlash + 1 : lastBackslash + 1;
        upload_filename = disp.substring(start);
        Serial.printf("[Upload] fallback: multipart filename -> '%s'\n", upload_filename.c_str());
      }
    } else {
      Serial.printf("[Upload] query string -> '%s'\n", upload_filename.c_str());
    }
    if (upload_filename.length() > MAX_IMG_NAME_LEN) {
      upload_filename = upload_filename.substring(0, MAX_IMG_NAME_LEN);
    }

    if (uploadFile != nullptr) {
      uploadFile->close();
      delete uploadFile;
      uploadFile = nullptr;
    }

    // 빈 슬롯을 찾아서 직접 /img/N.raw 로 저장
    upload_slot = -1;
    for (int i = 0; i < MAX_IMAGES; i++) {
      if (!image_slots_used[i]) {
        upload_slot = i;
        break;
      }
    }

    if (upload_slot == -1) {
      Serial.println("[Upload] 저장 공간이 가득 찼습니다!");
    } else {
      String dest = get_image_path(upload_slot);
      LittleFS.remove(dest);  // 기존 파일 정리
      File file = LittleFS.open(dest, "w");
      if (file) {
        uploadFile = new File(file);
        Serial.printf("[Upload] 슬롯 %d에 직접 저장 시작: %s\n", upload_slot, dest.c_str());
      } else {
        Serial.printf("[Upload] 파일 생성 실패: %s\n", dest.c_str());
        upload_slot = -1;
      }
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile && *uploadFile && upload_slot >= 0) {
      uploadFile->write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile->close();
      delete uploadFile;
      uploadFile = nullptr;
    }

    if (upload_slot >= 0) {
      // 메타데이터 업데이트
      image_slots_used[upload_slot] = true;
      if (upload_filename.length() > 0) {
        upload_filename.toCharArray(image_names[upload_slot], MAX_IMG_NAME_LEN + 1);
      } else {
        snprintf(image_names[upload_slot], MAX_IMG_NAME_LEN + 1, "image_%d.raw", upload_slot);
      }
      current_image_index = upload_slot;
      image_count = 0;
      for (int i = 0; i < MAX_IMAGES; i++) {
        if (image_slots_used[i]) image_count++;
      }
      save_image_info();
      Serial.printf("[Upload] 이미지 %d (%s) 저장 완료 (총 %d개)\n",
        upload_slot, image_names[upload_slot], image_count);
    }

    is_uploading = false;
    upload_filename = "";
    upload_slot = -1;
  }
}

// --- 웹서버 API 핸들러 ---
void handle_list_images() {
  String json;
  json.reserve(1024);  // heap 단편화 방지
  json = "{";
  json += "\"current\":" + String(current_image_index) + ",";
  json += "\"images\":[";
  bool first = true;
  for (int i = 0; i < MAX_IMAGES; i++) {
    if (image_slots_used[i]) {
      if (!first) json += ",";
      first = false;
      json += "{";
      json += "\"index\":" + String(i) + ",";
      json += "\"name\":\"" + String(image_names[i]) + "\"";
      json += "}";
    }
  }
  json += "]}";
  server.send(200, "application/json", json);
}

void handle_select_image() {
  if (!server.hasArg("index")) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"index required\"}");
    return;
  }

  int idx = server.arg("index").toInt();
  if (idx < 0 || idx >= MAX_IMAGES || !image_slots_used[idx]) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"invalid index\"}");
    return;
  }

  current_image_index = idx;
  save_image_info();
  load_image_to_sram(idx);

  String resp = "{\"status\":\"ok\",\"current\":";
  resp += idx;
  resp += "}";
  server.send(200, "application/json", resp);
  Serial.printf("[Web] 이미지 %d 선택됨 (%s)\n", idx, image_names[idx]);
}

void handle_delete_image() {
  if (!server.hasArg("index")) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"index required\"}");
    return;
  }

  int idx = server.arg("index").toInt();
  if (idx < 0 || idx >= MAX_IMAGES || !image_slots_used[idx]) {
    server.send(400, "application/json", "{\"status\":\"error\",\"message\":\"invalid index\"}");
    return;
  }

  bool was_current = (idx == current_image_index);
  delete_image(idx);

  // 현재 이미지가 삭제된 경우 다른 이미지로 전환
  if (was_current) {
    if (image_count > 0) {
      // 다음 사용 중인 슬롯 찾기 (idx+1 부터 순환)
      int next = -1;
      for (int i = 1; i <= MAX_IMAGES; i++) {
        int candidate = (idx + i) % MAX_IMAGES;
        if (image_slots_used[candidate]) {
          next = candidate;
          break;
        }
      }
      if (next >= 0) {
        current_image_index = next;
        load_image_to_sram(next);
        save_image_info();
      }
    } else {
      // 저장된 이미지가 없으면 버퍼 비우기
      current_image_index = 0;
      if (img_buffer != nullptr) {
        free(img_buffer);
        img_buffer = nullptr;
        img_width = 0;
      }
      clear_apa102_fast();
    }
  }

  server.send(200, "application/json", "{\"status\":\"ok\"}");
  Serial.printf("[Web] 이미지 %d 삭제됨\n", idx);
}

// --- 썸네일 데이터 API ---
void handle_image_data() {
  if (!server.hasArg("index")) {
    server.send(400, "text/plain", "index required");
    return;
  }
  int idx = server.arg("index").toInt();
  if (idx < 0 || idx >= MAX_IMAGES || !image_slots_used[idx]) {
    server.send(400, "text/plain", "invalid index");
    return;
  }

  String path = get_image_path(idx);
  File file = LittleFS.open(path, "r");
  if (!file) {
    server.send(404, "text/plain", "not found");
    return;
  }

  uint32_t fileSize = file.size();
  uint8_t* buf = (uint8_t*)malloc(fileSize);
  if (!buf) {
    file.close();
    server.send(500, "text/plain", "memory error");
    return;
  }
  file.read(buf, fileSize);
  file.close();

  server.setContentLength(fileSize);
  server.send(200, "application/octet-stream", "");
  server.sendContent((const char*)buf, fileSize);
  server.client().flush();
  free(buf);
}

// --- 버튼 처리 함수 ---
void indicate_image_index() {
  // 모든 LED 버퍼를 먼저 초기화 (이전 이미지 데이터 제거)
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i].r = 0;
    leds[i].g = 0;
    leds[i].b = 0;
  }

  // 하단 LED부터 (current_image_index + 1)개 점등
  int num_leds_on = current_image_index + 1;
  for (int i = 0; i < num_leds_on && i < NUM_LEDS; i++) {
    leds[i].r = 255;
    leds[i].g = 255;
    leds[i].b = 255;
  }
  show_apa102_fast();
  delay(500);
  clear_apa102_fast();
}

void cycle_to_next_image() {
  if (image_count == 0) {
    Serial.println("[Button] 저장된 이미지가 없습니다.");
    return;
  }
  if (image_count == 1) {
    Serial.println("[Button] 이미지가 1개뿐이라 순환하지 않습니다.");
    return;
  }

  // 현재 인덱스 이후로 순환하며 다음 사용 중인 슬롯 검색
  int next = -1;
  for (int i = 1; i <= MAX_IMAGES; i++) {
    int candidate = (current_image_index + i) % MAX_IMAGES;
    if (image_slots_used[candidate]) {
      next = candidate;
      break;
    }
  }

  if (next >= 0) {
    current_image_index = next;
    save_image_info();
    load_image_to_sram(next);
    indicate_image_index();
    Serial.printf("[Button] 이미지 %d (%s) 로 전환\n", next, image_names[next]);
  }
}

void setup() {
  Serial.begin(115200);

  // 온보드 LED 설정
  pinMode(ONBOARD_LED_PIN, OUTPUT);
  digitalWrite(ONBOARD_LED_PIN, LOW);

  // 스위치 설정
  pinMode(SW_PIN, INPUT_PULLUP);

  // 하드웨어 SPI 직접 가속 설정 (CLK=10, MISO=-1, DATA=6, CS=-1)
  SPI.begin(10, -1, 6, -1);
  SPI.setFrequency(20000000);  // 20MHz 초고속 설정

  // LittleFS 마운트
  if (!LittleFS.begin(true)) {
    Serial.println("LittleFS Mount Failed!");
  } else {
    Serial.println("LittleFS Mounted.");
  }

  // 다중 이미지 저장소 초기화
  ensure_img_dir();
  load_image_info();

  // 저장되어 있는 현재 이미지 로드
  load_image_to_sram(current_image_index);

  // Wi-Fi AP 설정
  WiFi.softAP(ap_ssid);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP Address: ");
  Serial.println(IP);

  // 웹서버 경로 바인딩
  server.on("/", HTTP_GET, []() {
    server.send_P(200, "text/html", INDEX_HTML);
  });
  // 바이너리 데이터 직접 스트리밍 업로드를 위한 핸들러
  server.on(
    "/upload", HTTP_POST, []() {
      // 업로드 완료 후 SRAM으로 이미지 로딩
      load_image_to_sram(current_image_index);
      server.send(200, "text/plain", "SUCCESS");
    },
    handle_image_upload);
  // 이미지 목록 API
  server.on("/list", HTTP_GET, handle_list_images);
  // 이미지 선택 API
  server.on("/select", HTTP_POST, handle_select_image);
  // 이미지 삭제 API
  server.on("/delete", HTTP_POST, handle_delete_image);
  // 썸네일 이미지 데이터 API
  server.on("/data", HTTP_GET, handle_image_data);

  server.begin();
  Serial.println("Web Server Started.");

  // MPU6050 및 I2C 초기화 (SDA 3, SCL 4)
  Wire.begin(3, 4);
  Wire.setClock(800000);  // 800kHz High Speed Mode
  mpu.initialize();

  if (!mpu.testConnection()) {
    Serial.println("MPU6050 connection failed!");
    // 센서 연결 실패 시 온보드 LED를 빠르게 깜빡여 에러 알림
    while (true) {
      digitalWrite(ONBOARD_LED_PIN, !digitalRead(ONBOARD_LED_PIN));
      delay(100);
    }
  }
  Serial.println("MPU6050 connection successful.");

  // 내부 Low Pass Filter 및 샘플 레이트 설정
  mpu.setDLPFMode(MPU6050_DLPF_BW_188);
  mpu.setRate(0);  // 1000Hz / (1+0) = 1000Hz 샘플링 (1ms 주기)
  mpu.setIntDataReadyEnabled(true);

  // ESP32C3 인터럽트 설정 (GPIO 0)
  pinMode(INTERRUPT_PIN, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), mpuInterruptHandler, RISING);
}
void clear_apa102_fast() {
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  SPI.write32(0);  // 시작 프레임
  for (int i = 0; i < NUM_LEDS; i++) {
    SPI.write32(0xE0000000);  // brightness=0 → LED OFF
  }
  // 종료 프레임 (APA102 사양: LED당 1/2비트 이상의 1 필요)
  for (int i = 0; i < (NUM_LEDS + 15) / 16; i++) SPI.write(0xFF);
  SPI.endTransaction();
}

// --- 하드웨어 SPI 직접 전송 기반 초고속 show 함수 ---
void show_apa102_fast() {
  SPI.beginTransaction(SPISettings(20000000, MSBFIRST, SPI_MODE0));
  // 시작 프레임: 32비트 0
  SPI.write32(0);
  // LED 데이터 전송 (Brightness 최대 + B, G, R 순서로 전송)
  for (int i = 0; i < NUM_LEDS; i++) {
    SPI.write(0xFF);  // Global Brightness 최대
    SPI.write(leds[i].b);
    SPI.write(leds[i].g);
    SPI.write(leds[i].r);
  }
  // 종료 프레임
  for (int i = 0; i < (NUM_LEDS + 15) / 16; i++) {
    SPI.write(0xFF);
  }
  SPI.endTransaction();
}

// --- POV 렌더링 함수 ---
void display_pov() {
  if (img_buffer == nullptr || img_width == 0) {
    // 소등 처리
    clear_apa102_fast();
    return;
  }

  unsigned long start_pov_ms = millis();  // 렌더링 시작 시간 기록 (디버깅용)

  // 스윙의 중앙 60% 영역을 활용하여 렌더링 (20% 여백 / 60% 표시 / 20% 여백 원복)
  unsigned long active_duration = (swing_duration * 60) / 100;
  unsigned long start_delay = (swing_duration * 20) / 100;
  unsigned long col_delay_us = (active_duration * 1000) / img_width;  // 마이크로초 단위 목표 딜레이

  // 여백 시간 대기
  if (start_delay > 0) {
    delay(start_delay);
  }

  // ═══ prev_gz_val을 현재 swing 방향으로 동기화 ═══
  // loop()의 prev_gz_val는 교차 직전 반대방향 값이므로, 이대로 검사하면
  // 첫 컬럼에서 무조건 부호불일치로 중단됨 → 현재 gz로 덮어쓰기
  {
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    prev_gz_val = gz;
  }

  // 방향에 맞춰 이미지를 정방향 또는 역방향 출력
  for (uint16_t col = 0; col < img_width; col++) {
    unsigned long col_start_us = micros();  // 각 열 출력 시작 시각 기록

    // 흔드는 방향이 우->좌 이면 이미지를 역순으로 뿌림 (좌우 반전 방지)
    uint16_t target_col = swing_direction ? col : (img_width - 1 - col);

    // SRAM 이미지 버퍼에서 72개 LED용 RGB 데이터를 leds[] 에 복사
    size_t col_offset = target_col * NUM_LEDS * 3;
    for (int i = 0; i < NUM_LEDS; i++) {
      size_t pixel_offset = col_offset + (i * 3);
      leds[i].r = img_buffer[pixel_offset];
      leds[i].g = img_buffer[pixel_offset + 1];
      leds[i].b = img_buffer[pixel_offset + 2];
    }

    // 초고속 하드웨어 SPI 직접 전송 호출
    show_apa102_fast();

    // ═══ 컬럼 출력 후 각속도 부호변화 감시: 방향이 바뀌면 중단 ═══
    // (최초 3개 컬럼은 건너뛰어 swing 진입 직후 오탐지 방지)
    if (col >= 3 && MPUInterrupt) {
      MPUInterrupt = false;
      int16_t ax, ay, az, gx, gy, gz;
      mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
      if ((prev_gz_val > 0 && gz < 0) || (prev_gz_val < 0 && gz > 0)) {
        if (abs(prev_gz_val - gz) > SWING_THRESHOLD) {
          // 스윙 방향 전환 감지 → 렌더링 중단
          prev_gz_val = gz;
          last_zero_crossing_time = millis();
          clear_apa102_fast();
          Serial.println("[POV] 중간 방향전환 감지 → 렌더링 중단");
          return;
        }
      }
      prev_gz_val = gz;
    }

    // 송신 지연 시간을 뺀 실질 시간만큼만 대기하여 정밀 시간 보상 복원
    long elapsed_us = micros() - col_start_us;
    long remaining_us = (long)col_delay_us - elapsed_us;
    if (remaining_us > 0) {
      delayMicroseconds(remaining_us);
    }
  }

  // 렌더링 완료 후 LED 소등
  clear_apa102_fast();
}


void loop() {
  // 웹서버 요청 처리
  server.handleClient();

  // 이미지를 업로드 중일 때는 CPU 자원 보호 및 메모리 간섭 방지를 위해 모션 루프를 생략합니다.
  if (is_uploading) {
    delay(10);
    return;
  }

  // --- 버튼 디바운스 처리 ---
  {
    bool reading = digitalRead(SW_PIN);
    if (reading != last_button_reading) {
      last_button_debounce_time = millis();  // 상태 변화 감지 → 타이머 리셋
    }
    if ((millis() - last_button_debounce_time) > DEBOUNCE_DELAY) {
      // DEBOUNCE_DELAY 동안 안정된 상태만 인정
      if (reading != button_state) {
        button_state = reading;
        if (button_state == LOW) {  // falling edge: 버튼 눌림
          button_pressed = true;
        }
      }
    }
    last_button_reading = reading;  // 이전 읽기값 갱신 (엣지 감지용)
  }

  if (button_pressed) {
    button_pressed = false;
    cycle_to_next_image();
  }

  // MPU6050의 하드웨어 데이터 준비 완료 인터럽트가 트리거되었을 때만 처리
  if (MPUInterrupt) {
    MPUInterrupt = false;

    // MPU6050 센서 리딩 (Raw 데이터 획득)
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    gz_val = gz;  // z축 각속도

    unsigned long current_time = millis();

    // 영점교차 + 델타 ≥ 400 → 스윙 방향 전환 감지
    if ((prev_gz_val > 0 && gz_val < 0) || (prev_gz_val < 0 && gz_val > 0)) {
      if (abs(prev_gz_val - gz_val) > SWING_THRESHOLD) {
        unsigned long duration = current_time - last_zero_crossing_time;

        // 판정 성공 여부와 상관없이 기준 시점을 즉시 갱신
        last_zero_crossing_time = current_time;

        // 500ms 편도 스윙 시간 필터링
        if (duration < 500) {
          swing_duration = duration;
          swing_direction = (gz_val < 0);  // 회전 부호에 따라 방향 결정

          Serial.printf("[POV] 스윙 디텍트! 방향: %s, 주기: %lu ms\n",
                        swing_direction ? "좌 -> 우" : "우 -> 좌", swing_duration);

          // POV 디스플레이 작동
          display_pov();
        }
      }
    }

    prev_gz_val = gz_val;

  }

}