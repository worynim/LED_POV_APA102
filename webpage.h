#ifndef WEBPAGE_H
#define WEBPAGE_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LED POV 스틱 이미지 업로더</title>
    <link href="https://fonts.googleapis.com/css2?family=Outfit:wght@300;400;600;800&display=swap" rel="stylesheet">
    <style>
        :root {
            --bg-color: #0b0c10;
            --card-bg: #1f2833;
            --accent-color: #66fcf1;
            --accent-hover: #45a29e;
            --text-color: #c5c6c7;
            --title-color: #ffffff;
            --border-radius: 16px;
        }

        * {
            box-sizing: border-box;
            margin: 0;
            padding: 0;
            font-family: 'Outfit', sans-serif;
        }

        body {
            background-color: var(--bg-color);
            color: var(--text-color);
            display: flex;
            justify-content: center;
            align-items: center;
            min-height: 100vh;
            padding: 2rem 1rem;
        }

        .container {
            width: 100%;
            max-width: 600px;
            background: var(--card-bg);
            border-radius: var(--border-radius);
            box-shadow: 0 8px 32px 0 rgba(0, 0, 0, 0.37);
            border: 1px solid rgba(255, 255, 255, 0.05);
            padding: 2.5rem;
            display: flex;
            flex-direction: column;
            gap: 2rem;
        }

        header {
            text-align: center;
        }

        h1 {
            font-size: 2.2rem;
            font-weight: 800;
            color: var(--title-color);
            margin-bottom: 0.5rem;
            letter-spacing: 1px;
            background: linear-gradient(45deg, var(--accent-color), #00d2ff);
            -webkit-background-clip: text;
            -webkit-text-fill-color: transparent;
        }

        p.subtitle {
            font-size: 0.95rem;
            color: #858994;
        }

        .drop-zone {
            border: 2px dashed rgba(102, 252, 241, 0.3);
            border-radius: var(--border-radius);
            padding: 2rem;
            text-align: center;
            cursor: pointer;
            transition: all 0.3s ease;
            background: rgba(255, 255, 255, 0.02);
        }

        .drop-zone:hover, .drop-zone.dragover {
            border-color: var(--accent-color);
            background: rgba(102, 252, 241, 0.05);
            box-shadow: 0 0 15px rgba(102, 252, 241, 0.1);
        }

        .drop-zone-icon {
            font-size: 3rem;
            margin-bottom: 1rem;
            color: var(--accent-color);
        }

        .drop-zone-text {
            font-weight: 500;
            color: var(--title-color);
        }

        #file-input {
            display: none;
        }

        .preview-section {
            display: none;
            flex-direction: column;
            gap: 1.5rem;
            animation: fadeIn 0.5s ease;
        }

        @keyframes fadeIn {
            from { opacity: 0; transform: translateY(10px); }
            to { opacity: 1; transform: translateY(0); }
        }

        .preview-title {
            font-weight: 600;
            color: var(--title-color);
            font-size: 1.1rem;
            border-left: 4px solid var(--accent-color);
            padding-left: 0.5rem;
        }

        .canvas-container {
            width: 100%;
            overflow-x: auto;
            background: #000;
            border-radius: 8px;
            padding: 1rem;
            display: flex;
            justify-content: center;
        }

        canvas {
            display: block;
            image-rendering: pixelated;
            max-height: 250px;
        }

        .info-grid {
            display: grid;
            grid-template-columns: repeat(2, 1fr);
            gap: 1rem;
            background: rgba(0, 0, 0, 0.15);
            padding: 1rem;
            border-radius: 8px;
            font-size: 0.9rem;
        }

        .info-item span:first-child {
            color: #858994;
            display: block;
            font-size: 0.8rem;
            margin-bottom: 0.2rem;
        }

        .info-item span:last-child {
            font-weight: 600;
            color: var(--title-color);
        }

        .btn {
            background: linear-gradient(135deg, var(--accent-color), var(--accent-hover));
            color: var(--bg-color);
            border: none;
            border-radius: var(--border-radius);
            padding: 1rem;
            font-size: 1.1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 4px 15px rgba(102, 252, 241, 0.25);
        }

        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(102, 252, 241, 0.4);
        }

        .btn:active {
            transform: translateY(0);
        }

        .btn:disabled {
            background: #2c3540;
            color: #606f7b;
            cursor: not-allowed;
            box-shadow: none;
            transform: none;
        }

        .progress-container {
            display: none;
            width: 100%;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            overflow: hidden;
            height: 8px;
        }

        .progress-bar {
            height: 100%;
            width: 0%;
            background: var(--accent-color);
            box-shadow: 0 0 10px var(--accent-color);
            transition: width 0.1s ease;
        }

        #status-message {
            text-align: center;
            font-weight: 500;
            font-size: 0.95rem;
        }

        .success { color: #2ecc71; }
        .error { color: #e74c3c; }
        .loading { color: var(--accent-color); }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>POV LED STICK</h1>
            <p class="subtitle">POV 스틱용 이미지를 업로드하고 리사이징합니다.</p>
        </header>

        <div class="drop-zone" id="drop-zone">
            <div class="drop-zone-icon">📁</div>
            <div class="drop-zone-text">이미지를 드래그하여 놓거나 클릭하여 선택하세요</div>
            <p class="subtitle" style="margin-top: 0.5rem;">세로 72픽셀로 자동 변환됩니다.</p>
            <input type="file" id="file-input" accept="image/*">
        </div>

        <div class="preview-section" id="preview-section">
            <div class="preview-title">LED 출력 미리보기</div>
            <div class="canvas-container">
                <canvas id="preview-canvas"></canvas>
            </div>
            <div class="info-grid">
                <div class="info-item">
                    <span>변환 해상도</span>
                    <span id="info-resolution">-</span>
                </div>
                <div class="info-item">
                    <span>추정 메모리</span>
                    <span id="info-memory">-</span>
                </div>
            </div>
            <button class="btn" id="upload-btn">POV 스틱에 전송하기</button>
        </div>

        <div class="progress-container" id="progress-container">
            <div class="progress-bar" id="progress-bar"></div>
        </div>
        <div id="status-message"></div>
    </div>

    <script>
        const dropZone = document.getElementById('drop-zone');
        const fileInput = document.getElementById('file-input');
        const previewSection = document.getElementById('preview-section');
        const previewCanvas = document.getElementById('preview-canvas');
        const infoResolution = document.getElementById('info-resolution');
        const infoMemory = document.getElementById('info-memory');
        const uploadBtn = document.getElementById('upload-btn');
        const progressContainer = document.getElementById('progress-container');
        const progressBar = document.getElementById('progress-bar');
        const statusMessage = document.getElementById('status-message');

        let rawBuffer = null;
        let imgWidth = 0;
        let imgHeight = 72; // Target POV height

        // 드래그 앤 드롭 이벤트
        ['dragenter', 'dragover'].forEach(eventName => {
            dropZone.addEventListener(eventName, (e) => {
                e.preventDefault();
                dropZone.classList.add('dragover');
            }, false);
        });

        ['dragleave', 'drop'].forEach(eventName => {
            dropZone.addEventListener(eventName, (e) => {
                e.preventDefault();
                dropZone.classList.remove('dragover');
            }, false);
        });

        dropZone.addEventListener('drop', (e) => {
            const dt = e.dataTransfer;
            const files = dt.files;
            if (files.length > 0) {
                handleFile(files[0]);
            }
        });

        dropZone.addEventListener('click', () => {
            fileInput.click();
        });

        fileInput.addEventListener('change', (e) => {
            if (e.target.files.length > 0) {
                handleFile(e.target.files[0]);
            }
        });

        function handleFile(file) {
            if (!file.type.startsWith('image/')) {
                showStatus('이미지 파일만 지원합니다.', 'error');
                return;
            }

            showStatus('이미지 처리 중...', 'loading');
            const reader = new FileReader();
            reader.readAsDataURL(file);
            reader.onload = function(event) {
                const img = new Image();
                img.src = event.target.result;
                img.onload = function() {
                    processImage(img);
                };
            };
        }
        // RGB를 HSV로 변환하는 함수 (R, G, B 입력 범위: 0~255)
        function rgbToHsv(r, g, b) {
            r /= 255; g /= 255; b /= 255;
            const max = Math.max(r, g, b), min = Math.min(r, g, b);
            let h, s, v = max;
            const d = max - min;
            s = max === 0 ? 0 : d / max;

            if (max === min) {
                h = 0;
            } else {
                switch (max) {
                    case r: h = (g - b) / d + (g < b ? 6 : 0); break;
                    case g: h = (b - r) / d + 2; break;
                    case b: h = (r - g) / d + 4; break;
                }
                h /= 6;
            }
            return { h, s, v };
        }

        // HSV를 RGB로 변환하는 함수 (H, S, V 입력 범위: 0~1)
        function hsvToRgb(h, s, v) {
            let r, g, b;
            const i = Math.floor(h * 6);
            const f = h * 6 - i;
            const p = v * (1 - s);
            const q = v * (1 - f * s);
            const t = v * (1 - (1 - f) * s);

            switch (i % 6) {
                case 0: r = v, g = t, b = p; break;
                case 1: r = q, g = v, b = p; break;
                case 2: r = p, g = v, b = t; break;
                case 3: r = p, g = q, b = v; break;
                case 4: r = t, g = p, b = v; break;
                case 5: r = v, g = p, b = q; break;
            }
            return [Math.round(r * 255), Math.round(g * 255), Math.round(b * 255)];
        }
        function processImage(img) {
            // 세로 72px 기준 가로 비율 계산
            const scale = imgHeight / img.height;
            imgWidth = Math.round(img.width * scale);
            
            // 최대 폭을 300px 정도로 제한 (메모리 및 POV 가독성 방지)
            if (imgWidth > 300) {
                imgWidth = 300;
            }
            if (imgWidth < 1) {
                imgWidth = 1;
            }

            previewCanvas.width = imgWidth;
            previewCanvas.height = imgHeight;
            const ctx = previewCanvas.getContext('2d');
            
            // 이미지 부드럽게 리사이징 비활성화 (픽셀아트 느낌 유지)
            ctx.imageSmoothingEnabled = false;
            ctx.drawImage(img, 0, 0, imgWidth, imgHeight);

            // 픽셀 데이터 획득
            const imgData = ctx.getImageData(0, 0, imgWidth, imgHeight);
            const data = imgData.data;

            // ESP32 전송용 raw 데이터 포맷 생성
            // 헤더: 2바이트 가로 폭 (Width) - Big Endian
            // 바디: Column 단위 순회하면서 RGB값 추출
            // 첫번째 LED가 아래(Index 71), 마지막 LED가 위(Index 0) -> 세로 방향 뒤집어서 전송 필요
            // 혹은 루프 돌릴 때 아래(Y=71)부터 위(Y=0) 방향으로 버퍼링
            const bodySize = imgWidth * imgHeight * 3;
            rawBuffer = new Uint8Array(2 + bodySize);

            // 헤더 작성 (2바이트 가로폭)
            rawBuffer[0] = (imgWidth >> 8) & 0xFF;
            rawBuffer[1] = imgWidth & 0xFF;
/*
            let bufferIdx = 2;
            for (let x = 0; x < imgWidth; x++) {
                // 첫번째 LED가 아래(Y = 71)에 위치하므로, Y 루프를 71부터 0으로 역순 순회
                for (let y = imgHeight - 1; y >= 0; y--) {
                    const pixelIdx = (y * imgWidth + x) * 4;
                    rawBuffer[bufferIdx++] = data[pixelIdx];     // Red
                    rawBuffer[bufferIdx++] = data[pixelIdx + 1]; // Green
                    rawBuffer[bufferIdx++] = data[pixelIdx + 2]; // Blue
                }
            }
*/
            let bufferIdx = 2;
            for (let x = 0; x < imgWidth; x++) {
                for (let y = imgHeight - 1; y >= 0; y--) {
                    const pixelIdx = (y * imgWidth + x) * 4;
                    
                    // 원본 RGB 데이터 추출
                    let r = data[pixelIdx];
                    let g = data[pixelIdx + 1];
                    let b = data[pixelIdx + 2];

                    // 1. HSV 공간으로 변환
                    let hsv = rgbToHsv(r, g, b);

                    // 2. 채도 증폭 & 밝기 감소 필터 적용
                    const SATURATION_SCALE = 1.5; // 채도 증가 배율 (1.0이 원본, 숫자가 클수록 진해짐)
                    const BRIGHTNESS_SCALE = 0.7; // 밝기 감소 배율 (1.0이 원본, 0.5는 밝기를 절반으로 줄임)

                    hsv.s = Math.min(1.0, hsv.s * SATURATION_SCALE); // 채도를 높이되 최대치 1.0 제한
                    hsv.v = hsv.v * BRIGHTNESS_SCALE;               // 밝기 감소

                    // 3. 다시 RGB로 복원
                    const [targetR, targetG, targetB] = hsvToRgb(hsv.h, hsv.s, hsv.v);

                    // 변환된 데이터를 버퍼에 저장
                    rawBuffer[bufferIdx++] = targetR; // Red
                    rawBuffer[bufferIdx++] = targetG; // Green
                    rawBuffer[bufferIdx++] = targetB; // Blue
                }
            }
            // 정보 표시
            infoResolution.textContent = `${imgWidth} x ${imgHeight} px`;
            infoMemory.textContent = `${Math.round(rawBuffer.length / 1024 * 10) / 10} KB`;
            
            previewSection.style.display = 'flex';
            showStatus('이미지 분석 완료. POV 스틱에 전송할 준비가 되었습니다.', 'success');
        }

        function showStatus(text, type) {
            statusMessage.textContent = text;
            statusMessage.className = type || '';
        }

        uploadBtn.addEventListener('click', () => {
            if (!rawBuffer) return;

            uploadBtn.disabled = true;
            progressContainer.style.display = 'block';
            progressBar.style.width = '0%';
            showStatus('POV 스틱에 전송 중...', 'loading');

            // Raw binary를 Blob으로 감싸고 FormData를 통해 전송 (multipart/form-data 호환성 확보)
            const blob = new Blob([rawBuffer], { type: 'application/octet-stream' });
            const formData = new FormData();
            formData.append('file', blob, 'image.raw');

            const xhr = new XMLHttpRequest();
            xhr.open('POST', '/upload', true);

            xhr.upload.onprogress = function(e) {
                if (e.lengthComputable) {
                    const percent = (e.loaded / e.total) * 100;
                    progressBar.style.width = percent + '%';
                }
            };

            xhr.onload = function() {
                uploadBtn.disabled = false;
                if (xhr.status === 200) {
                    showStatus('업로드 완료! POV 스틱에 이미지가 반영되었습니다.', 'success');
                } else {
                    showStatus('업로드 실패: ' + xhr.statusText, 'error');
                }
            };

            xhr.onerror = function() {
                uploadBtn.disabled = false;
                showStatus('네트워크 에러가 발생했습니다.', 'error');
            };

            xhr.send(formData);
        });
    </script>
</body>
</html>
)rawliteral";

#endif

