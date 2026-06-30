#ifndef WEBPAGE_H
#define WEBPAGE_H

const char INDEX_HTML[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="ko">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>LED POV 스틱 이미지 업로더</title>
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
            font-family: -apple-system, BlinkMacSystemFont, 'Segoe UI', system-ui, sans-serif;
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

        /* --- 이미지 이름 입력 (모바일 대응) --- */
        .name-input-group {
            display: none;
            flex-direction: column;
            gap: 0.5rem;
        }
        .name-input-group label {
            font-size: 0.85rem;
            color: var(--accent-color);
            font-weight: 500;
        }
        .name-input-group input {
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 0.7rem 1rem;
            color: var(--title-color);
            font-size: 0.95rem;
            outline: none;
            transition: border-color 0.3s;
        }
        .name-input-group input:focus {
            border-color: var(--accent-color);
        }

        /* --- 저장된 이미지 목록 --- */
        .image-list-section {
            display: none;
            flex-direction: column;
            gap: 0.8rem;
            animation: fadeIn 0.5s ease;
        }
        .image-list {
            display: flex;
            flex-direction: column;
            gap: 0.5rem;
        }
        .image-item {
            display: flex;
            align-items: center;
            justify-content: space-between;
            background: rgba(0, 0, 0, 0.2);
            border: 1px solid rgba(255, 255, 255, 0.06);
            border-radius: 10px;
            padding: 0.6rem 1rem;
            transition: all 0.2s ease;
        }
        .image-item.current {
            border-color: var(--accent-color);
            background: rgba(102, 252, 241, 0.08);
        }
        .image-item .thumb {
            width: 72px;
            height: 72px;
            border-radius: 4px;
            background: #000;
            flex-shrink: 0;
            margin-right: 0.8rem;
            image-rendering: pixelated;
            border: 1px solid rgba(255,255,255,0.08);
        }
        .image-item .info {
            flex: 1;
            min-width: 0;
            display: flex;
            flex-direction: column;
            gap: 0.15rem;
        }
        .image-item .name {
            font-weight: 500;
            color: var(--title-color);
            font-size: 0.9rem;
            overflow: hidden;
            text-overflow: ellipsis;
            white-space: nowrap;
        }
        .image-item .dims {
            font-size: 0.7rem;
            color: #858994;
        }
        .image-item .badge {
            font-size: 0.7rem;
            background: var(--accent-color);
            color: var(--bg-color);
            border-radius: 6px;
            padding: 0.15rem 0.5rem;
            font-weight: 600;
            margin-left: 0.5rem;
        }
        .image-item .actions {
            display: flex;
            gap: 0.4rem;
            flex-shrink: 0;
        }
        .image-item .actions button {
            border: none;
            border-radius: 6px;
            padding: 0.3rem 0.7rem;
            font-size: 0.8rem;
            font-weight: 500;
            cursor: pointer;
            transition: all 0.2s;
        }
        .btn-select {
            background: var(--accent-color);
            color: var(--bg-color);
        }
        .btn-select:hover {
            background: var(--accent-hover);
        }
        .btn-delete {
            background: rgba(231, 76, 60, 0.2);
            color: #e74c3c;
        }
        .btn-delete:hover {
            background: rgba(231, 76, 60, 0.4);
        }
        .btn-select:disabled {
            background: #2c3540;
            color: #606f7b;
            cursor: not-allowed;
        }
        .no-images {
            color: #858994;
            text-align: center;
            padding: 1.5rem;
            font-size: 0.9rem;
        }

        /* --- 현재 디스플레이 정보 --- */
        .current-info {
            display: none;
            flex-direction: column;
            gap: 0.5rem;
            animation: fadeIn 0.5s ease;
        }
        .current-display {
            background: rgba(102, 252, 241, 0.1);
            border: 1px solid var(--accent-color);
            border-radius: 10px;
            padding: 0.8rem 1rem;
            display: flex;
            align-items: center;
            gap: 0.5rem;
        }
        .current-display .label {
            font-size: 0.8rem;
            color: #858994;
            font-weight: 500;
        }
        .current-display #current-name {
            font-weight: 700;
            color: var(--accent-color);
            font-size: 1rem;
            flex: 1;
        }
        .current-display .current-badge {
            font-size: 0.7rem;
            background: var(--accent-color);
            color: var(--bg-color);
            border-radius: 6px;
            padding: 0.2rem 0.6rem;
            font-weight: 700;
            text-transform: uppercase;
        }
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
            <div class="name-input-group" id="name-input-group">
                <label>저장할 이름 <span style="color:#858994;font-weight:400;">(수정 가능)</span></label>
                <input type="text" id="image-name" placeholder="파일명을 입력하세요" maxlength="24">
            </div>
            <button class="btn" id="upload-btn">POV 스틱에 전송하기</button>
        </div>

        <div class="progress-container" id="progress-container">
            <div class="progress-bar" id="progress-bar"></div>
        </div>
        <div id="status-message"></div>

        <div class="image-list-section" id="image-list-section">
            <div class="preview-title">저장된 이미지</div>
            <div class="image-list" id="image-list">
                <!-- JS로 동적 생성 -->
            </div>
            <div class="no-images" id="no-images" style="display:none;">
                저장된 이미지가 없습니다. 이미지를 업로드해주세요.
            </div>
        </div>

        <!-- 현재 디스플레이 정보 -->
        <div class="current-info" id="current-info" style="display:none;">
            <div class="preview-title">현재 디스플레이</div>
            <div id="current-display" class="current-display">
                <span id="current-name">-</span>
            </div>
        </div>
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
        let originalFileName = '';

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

            // 원본 파일명 저장
            originalFileName = file.name;
            if (originalFileName.length > 24) {
                originalFileName = originalFileName.substring(0, 24);
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

            // 헤더 작성 (2바이트 가로폭, Big Endian)
            rawBuffer[0] = (imgWidth >> 8) & 0xFF;
            rawBuffer[1] = imgWidth & 0xFF;

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
            // 파일명 입력 필드 표시 + 자동 채우기
            const nameInput = document.getElementById('image-name');
            const nameGroup = document.getElementById('name-input-group');
            nameInput.value = originalFileName;
            nameGroup.style.display = 'flex';
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

            // 입력 필드에서 최종 파일명 가져오기 (모바일에서 숫자 파일명 수정 가능)
            let finalName = document.getElementById('image-name').value.trim();
            if (!finalName) finalName = originalFileName;

            const file = new File([rawBuffer], finalName, { type: 'application/octet-stream' });
            const formData = new FormData();
            formData.append('file', file);

            const xhr = new XMLHttpRequest();
            xhr.open('POST', '/upload?name=' + encodeURIComponent(finalName), true);

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
                    fetchImageList();  // 목록 새로고침
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

        // --- 이미지 목록 API 함수 ---
        function fetchImageList() {
            fetch('/list')
                .then(response => response.json())
                .then(data => { renderImageList(data); })
                .catch(err => { console.error('이미지 목록 불러오기 실패:', err); });
        }

        function renderImageList(data) {
            const listEl = document.getElementById('image-list');
            const noImagesEl = document.getElementById('no-images');
            const sectionEl = document.getElementById('image-list-section');
            const currentInfoEl = document.getElementById('current-info');
            const currentNameEl = document.getElementById('current-name');
            sectionEl.style.display = 'flex';

            if (!data.images || data.images.length === 0) {
                listEl.innerHTML = '';
                noImagesEl.style.display = 'block';
                currentInfoEl.style.display = 'none';
                return;
            }

            noImagesEl.style.display = 'none';
            listEl.innerHTML = '';

            // 현재 이미지 정보 표시
            const currentImg = data.images.find(img => img.index === data.current);
            if (currentImg) {
                currentNameEl.textContent = currentImg.name + ' (' + (data.current + 1) + '/' + data.images.length + ')';
                currentInfoEl.style.display = 'flex';
            } else {
                currentInfoEl.style.display = 'none';
            }

            data.images.forEach(img => {
                const item = document.createElement('div');
                item.className = 'image-item';
                if (img.index === data.current) item.classList.add('current');

                // 썸네일 캔버스
                const thumbCanvas = document.createElement('canvas');
                thumbCanvas.className = 'thumb';
                thumbCanvas.width = 72;
                thumbCanvas.height = 72;

                const infoDiv = document.createElement('div');
                infoDiv.className = 'info';

                const nameSpan = document.createElement('span');
                nameSpan.className = 'name';
                nameSpan.textContent = img.name;

                const dimsSpan = document.createElement('span');
                dimsSpan.className = 'dims';
                dimsSpan.textContent = '로딩 중...';

                infoDiv.appendChild(nameSpan);
                infoDiv.appendChild(dimsSpan);

                const actions = document.createElement('div');
                actions.className = 'actions';

                const selectBtn = document.createElement('button');
                selectBtn.className = 'btn-select';
                selectBtn.textContent = '선택';
                selectBtn.disabled = (img.index === data.current);
                selectBtn.addEventListener('click', () => selectImage(img.index));

                const deleteBtn = document.createElement('button');
                deleteBtn.className = 'btn-delete';
                deleteBtn.textContent = '삭제';
                deleteBtn.addEventListener('click', () => deleteImage(img.index));

                actions.appendChild(selectBtn);
                actions.appendChild(deleteBtn);
                item.appendChild(thumbCanvas);
                item.appendChild(infoDiv);
                item.appendChild(actions);
                listEl.appendChild(item);

                // 썸네일 비동기 로드
                loadThumbnail(img.index, thumbCanvas, dimsSpan);
            });
        }

        function loadThumbnail(index, canvas, dimsSpan) {
            fetch('/data?index=' + index)
                .then(response => {
                    if (!response.ok) throw new Error('fail');
                    return response.arrayBuffer();
                })
                .then(buffer => {
                    const bytes = new Uint8Array(buffer);
                    if (bytes.length < 3) return;
                    // 헤더: 2바이트 가로 폭 (Big Endian)
                    const w = (bytes[0] << 8) | bytes[1];
                    const h = 72;
                    const dataLen = w * h * 3;
                    if (bytes.length < dataLen + 2) return;

                    dimsSpan.textContent = w + ' x ' + h + ' px';

                    // 캔버스 크기: 비율 유지하며 72px 높이에 맞춤
                    const thumbH = 72;
                    const thumbW = Math.max(1, Math.round(w * thumbH / h));
                    canvas.width = thumbW;
                    canvas.height = thumbH;

                    const ctx = canvas.getContext('2d');
                    const imgData = ctx.createImageData(thumbW, thumbH);

                    // 이미지 데이터를 썸네일 크기로 샘플링 (Y축 반전: LED0=하단)
                    for (let ty = 0; ty < thumbH; ty++) {
                        const srcY = h - 1 - Math.floor(ty * h / thumbH);
                        for (let tx = 0; tx < thumbW; tx++) {
                            const srcX = Math.floor(tx * w / thumbW);
                            const srcIdx = 2 + (srcX * h + srcY) * 3;
                            const dstIdx = (ty * thumbW + tx) * 4;
                            imgData.data[dstIdx] = bytes[srcIdx];       // R
                            imgData.data[dstIdx + 1] = bytes[srcIdx + 1]; // G
                            imgData.data[dstIdx + 2] = bytes[srcIdx + 2]; // B
                            imgData.data[dstIdx + 3] = 255;               // A
                        }
                    }
                    ctx.putImageData(imgData, 0, 0);
                })
                .catch(() => {
                    dimsSpan.textContent = '불러오기 실패';
                });
        }

        function selectImage(index) {
            showStatus('이미지 ' + (index + 1) + '번 선택 중...', 'loading');
            fetch('/select', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: 'index=' + index
            })
            .then(r => r.json())
            .then(data => {
                if (data.status === 'ok') {
                    showStatus('이미지 ' + (index + 1) + '번이 선택되었습니다.', 'success');
                    fetchImageList();
                } else {
                    showStatus('선택 실패: ' + (data.message || ''), 'error');
                }
            })
            .catch(() => showStatus('네트워크 에러', 'error'));
        }

        function deleteImage(index) {
            if (!confirm('이미지 ' + (index + 1) + '번을 삭제하시겠습니까?')) return;
            showStatus('이미지 삭제 중...', 'loading');
            fetch('/delete', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: 'index=' + index
            })
            .then(r => r.json())
            .then(data => {
                if (data.status === 'ok') {
                    showStatus('이미지가 삭제되었습니다.', 'success');
                    fetchImageList();
                } else {
                    showStatus('삭제 실패: ' + (data.message || ''), 'error');
                }
            })
            .catch(() => showStatus('네트워크 에러', 'error'));
        }

        // 페이지 로드 시 이미지 목록 불러오기
        window.addEventListener('load', fetchImageList);
    </script>
</body>
</html>
)rawliteral";

#endif

