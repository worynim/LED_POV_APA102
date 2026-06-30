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


// --- 이미지 SRAM 버퍼 ---
uint16_t img_width = 0;
uint8_t* img_buffer = nullptr;
const char* IMAGE_FILE_PATH = "/image.raw";

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

// --- 이미지 업로드 임시 파일 포인터 및 상태 플래그 ---
File* uploadFile = nullptr;
volatile bool is_uploading = false;

// --- LittleFS에서 이미지를 SRAM으로 적재하는 함수 ---
void load_image_to_sram() {
  if (img_buffer != nullptr) {
    free(img_buffer);
    img_buffer = nullptr;
    img_width = 0;
  }

  if (!LittleFS.exists(IMAGE_FILE_PATH)) {
    Serial.println("[LittleFS] 이미지 파일이 존재하지 않습니다. 대기 모드.");
    return;
  }

  File file = LittleFS.open(IMAGE_FILE_PATH, "r");
  if (!file) {
    Serial.println("[LittleFS] 이미지 파일 열기 실패.");
    return;
  }

  // 1. 가로 해상도(Width) 읽기 (2바이트, Big Endian)
  if (file.available() >= 2) {
    uint8_t h_byte = file.read();
    uint8_t l_byte = file.read();
    img_width = (h_byte << 8) | l_byte;
  }

  if (img_width == 0 || img_width > 300) {
    Serial.printf("[LittleFS] 유효하지 않은 가로 크기: %d px\n", img_width);
    file.close();
    return;
  }

  // 2. RGB 데이터 영역 메모리 할당 (Width * 72 * 3 bytes)
  size_t data_size = img_width * NUM_LEDS * 3;
  img_buffer = (uint8_t*)malloc(data_size);
  if (img_buffer == nullptr) {
    Serial.println("[Memory] SRAM 메모리 할당 실패!");
    img_width = 0;
    file.close();
    return;
  }

  // 3. 파일에서 데이터를 SRAM으로 한 번에 읽기
  size_t read_bytes = file.read(img_buffer, data_size);
  file.close();

  Serial.printf("[LittleFS] 이미지 로드 성공: %d x 72 px (%d bytes 읽음)\n", img_width, read_bytes);
}

// --- HTTP POST 업로드 핸들러 ---
void handle_image_upload() {
  HTTPUpload& upload = server.upload();

  if (upload.status == UPLOAD_FILE_START) {
    Serial.println("[Upload] 이미지 업로드 시작...");
    is_uploading = true;

    if (uploadFile != nullptr) {
      uploadFile->close();
      delete uploadFile;
      uploadFile = nullptr;
    }

    // 파일 쓰기 모드로 생성
    File file = LittleFS.open(IMAGE_FILE_PATH, "w");
    if (file) {
      uploadFile = new File(file);
    } else {
      Serial.println("[Upload] LittleFS 쓰기 파일 생성 실패.");
    }
  } else if (upload.status == UPLOAD_FILE_WRITE) {
    if (uploadFile && *uploadFile) {
      uploadFile->write(upload.buf, upload.currentSize);
    }
  } else if (upload.status == UPLOAD_FILE_END) {
    if (uploadFile) {
      uploadFile->close();
      delete uploadFile;
      uploadFile = nullptr;
      Serial.println("[Upload] 파일 저장 완료.");
    }
    is_uploading = false;
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

  // 저장되어 있는 기존 이미지 로드
  load_image_to_sram();

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
      // 업로드가 안전하게 끝난 후에 SRAM으로 이미지를 로딩하고 응답을 보냅니다.
      load_image_to_sram();
      server.send(200, "text/plain", "SUCCESS");
    },
    handle_image_upload);

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
  SPI.write32(0);
  for (int i = 0; i < NUM_LEDS; i++) {
    SPI.write32(0xE0000000);
  }
  // 종료 프레임
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
    delayMicroseconds(start_delay * 1000);
  }

  // 방향에 맞춰 이미지를 정방향 또는 역방향 출력
  for (uint16_t col = 0; col < img_width; col++) {
    unsigned long col_start_us = micros();  // 각 열 출력 시작 시각 기록

    // 흔드는 방향이 우->좌 이면 이미지를 역순으로 뿌림 (좌우 반전 방지)
    uint16_t target_col = swing_direction ? col : (img_width - 1 - col);

    // SRAM 이미지 버퍼에서 72개 LED용 RGB 데이터를 가져와 FastLED 버퍼에 쓰기
    size_t col_offset = target_col * NUM_LEDS * 3;
    for (int i = 0; i < NUM_LEDS; i++) {
      size_t pixel_offset = col_offset + (i * 3);
      leds[i].r = img_buffer[pixel_offset];
      leds[i].g = img_buffer[pixel_offset + 1];
      leds[i].b = img_buffer[pixel_offset + 2];
    }

    // 초고속 하드웨어 SPI 직접 전송 호출
    show_apa102_fast();

    // if (MPUInterrupt){  // 출력중에 인터럽트 걸리면 빠져 나가기
    //   clear_apa102_fast();
    //   return;
    // }

    // 송신 지연 시간을 뺀 실질 시간만큼만 대기하여 정밀 시간 보상 복원
    long elapsed_us = micros() - col_start_us;
    long remaining_us = (long)col_delay_us - elapsed_us;
    if (remaining_us > 0) {
      delayMicroseconds(remaining_us);
    }
  }

  // 렌더링 완료 후 LED 소등
  clear_apa102_fast();

  // 실행 시간 디버그 로그 출력
  unsigned long duration_ms = millis() - start_pov_ms;
  Serial.printf("[POV] display_pov() 실제 실행 시간: %lu ms (목표 swing_duration: %lu ms)\n", duration_ms, swing_duration);
}


// 마지막 스윙 감지 이후 경과 시간 (LED 자동 소등용)
unsigned long last_swing_time = 0;
const unsigned long IDLE_TIMEOUT = 1000;  // 1초 동안 스윙 없으면 LED 소등

void loop() {
  // 웹서버 요청 처리
  server.handleClient();

  // 이미지를 업로드 중일 때는 CPU 자원 보호 및 메모리 간섭 방지를 위해 모션 루프를 생략합니다.
  if (is_uploading) {
    delay(10);
    return;
  }

  // MPU6050의 하드웨어 데이터 준비 완료 인터럽트가 트리거되었을 때만 처리
  if (MPUInterrupt) {
    MPUInterrupt = false;

    // MPU6050 센서 리딩 (Raw 데이터 획득)
    int16_t ax, ay, az, gx, gy, gz;
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    gz_val = gz;  // z축 각속도

    unsigned long current_time = millis();

    // 부호가 바뀐 순간, 그 차이의 절대값이 1000이 넘는지 판단
    if ((prev_gz_val > 0 && gz_val < 0) || (prev_gz_val < 0 && gz_val > 0)) {
      if (abs(prev_gz_val - gz_val) > 400) {
        unsigned long duration = current_time - last_zero_crossing_time;

        // 판정 성공 여부와 상관없이 기준 시점을 즉시 갱신
        last_zero_crossing_time = current_time;

        // // 최소 80ms ~ 최대 800ms 편도 스윙 시간 필터링
        if (duration < 500) {
          swing_duration = duration;
          swing_direction = (gz_val < 0);  // 회전 부호에 따라 방향 결정
                                           //last_swing_time = current_time;  // 마지막 유효 스윙 시간 기록

          Serial.printf("[POV] 스윙 디텍트! 방향: %s, 주기: %lu ms\n",
                        swing_direction ? "좌 -> 우" : "우 -> 좌", swing_duration);

          // POV 디스플레이 작동
          display_pov();
        }
      }
    }

    prev_gz_val = gz_val;
    //Serial.println(prev_gz_val);
  }
  /*
  // 마지막 스윙으로부터 IDLE_TIMEOUT 동안 감지 없으면 LED 소등
  if (last_swing_time > 0 && (millis() - last_swing_time > IDLE_TIMEOUT)) {
    last_swing_time = 0;
    // 재활성화 시 첫 스윙부터 바로 감지되도록 기준 시간을 현재로 초기화
    last_zero_crossing_time = millis();
    FastLED.clear();
    show_apa102_fast();
    Serial.println("[POV] 정지 감지 -> LED 소등");
  }
   */
}