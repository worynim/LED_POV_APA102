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

        .color-adjust-section {
            display: none;
            flex-direction: column;
            gap: 0.9rem;
            background: rgba(0, 0, 0, 0.15);
            padding: 1rem;
            border-radius: 8px;
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

        /* --- 모드 전환 탭 --- */
        .mode-tabs {
            display: flex;
            gap: 0.5rem;
            background: rgba(0, 0, 0, 0.2);
            border-radius: 12px;
            padding: 0.3rem;
        }
        .mode-tab {
            flex: 1;
            background: transparent;
            color: #858994;
            border: none;
            border-radius: 10px;
            padding: 0.65rem 1rem;
            font-size: 0.95rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.25s ease;
        }
        .mode-tab:hover {
            color: var(--title-color);
        }
        .mode-tab.active {
            background: var(--accent-color);
            color: var(--bg-color);
            box-shadow: 0 2px 8px rgba(102, 252, 241, 0.3);
        }

        /* --- 텍스트 모드 패널 --- */
        .text-mode-panel {
            display: none;
            flex-direction: column;
            gap: 1.2rem;
            animation: fadeIn 0.4s ease;
        }
        .control-group {
            display: flex;
            flex-direction: column;
            gap: 0.4rem;
        }
        .control-group label {
            font-size: 0.85rem;
            color: var(--accent-color);
            font-weight: 500;
        }
        .control-group textarea,
        .control-group select {
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 0.7rem 1rem;
            color: var(--title-color);
            font-size: 0.95rem;
            outline: none;
            transition: border-color 0.3s;
            font-family: inherit;
            resize: none;
        }
        .control-group textarea:focus,
        .control-group select:focus {
            border-color: var(--accent-color);
        }
        .control-group select option {
            background: var(--card-bg);
            color: var(--text-color);
        }
        .control-group input[type="color"] {
            width: 100%;
            height: 42px;
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            background: rgba(0, 0, 0, 0.3);
            cursor: pointer;
            padding: 3px;
        }
        .control-group input[type="range"] {
            -webkit-appearance: none;
            width: 100%;
            height: 6px;
            border-radius: 3px;
            background: rgba(255, 255, 255, 0.15);
            outline: none;
        }
        .control-group input[type="range"]::-webkit-slider-thumb {
            -webkit-appearance: none;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            background: var(--accent-color);
            cursor: pointer;
            box-shadow: 0 0 8px rgba(102, 252, 241, 0.4);
        }
        .control-row {
            display: flex;
            gap: 1rem;
        }
        .control-half {
            flex: 1;
        }
        .char-counter {
            font-size: 0.8rem;
            color: #858994;
            text-align: right;
        }
        .char-counter.warn {
            color: #f39c12;
        }
        .char-counter.over {
            color: #e74c3c;
            font-weight: 600;
        }

        .font-input-row {
            display: flex;
            gap: 0.4rem;
        }
        .font-input-row input {
            flex: 1;
        }
        .font-input-row select {
            flex: 1;
            min-width: 0;
        }
        .btn-load-font {
            background: rgba(102, 252, 241, 0.15);
            border: 1px solid rgba(102, 252, 241, 0.3);
            border-radius: 10px;
            color: var(--accent-color);
            font-size: 1.1rem;
            padding: 0 0.8rem;
            cursor: pointer;
            transition: all 0.2s ease;
            white-space: nowrap;
        }
        .btn-load-font:hover {
            background: rgba(102, 252, 241, 0.25);
            border-color: var(--accent-color);
        }
        /* --- Wi-Fi 설정 패널 --- */
        .wifi-panel {
            display: none;
            flex-direction: column;
            gap: 1.2rem;
            animation: fadeIn 0.4s ease;
        }
        .wifi-status-bar {
            display: flex;
            align-items: center;
            gap: 0.8rem;
            background: rgba(0, 0, 0, 0.2);
            border-radius: 10px;
            padding: 0.8rem 1rem;
        }
        .wifi-mode-badge {
            font-size: 0.75rem;
            font-weight: 700;
            padding: 0.2rem 0.6rem;
            border-radius: 6px;
            text-transform: uppercase;
            flex-shrink: 0;
        }
        .badge-sta {
            background: rgba(46, 204, 113, 0.2);
            color: #2ecc71;
            border: 1px solid rgba(46, 204, 113, 0.4);
        }
        .badge-ap {
            background: rgba(241, 196, 15, 0.2);
            color: #f1c40f;
            border: 1px solid rgba(241, 196, 15, 0.4);
        }
        .wifi-ip-text {
            font-family: monospace;
            font-size: 0.9rem;
            color: var(--title-color);
            flex: 1;
            overflow: hidden;
            text-overflow: ellipsis;
            white-space: nowrap;
        }
        .wifi-ip-copy {
            background: transparent;
            border: 1px solid rgba(255,255,255,0.15);
            border-radius: 6px;
            color: #858994;
            padding: 0.25rem 0.6rem;
            font-size: 0.8rem;
            cursor: pointer;
            transition: all 0.2s;
            flex-shrink: 0;
        }
        .wifi-ip-copy:hover {
            border-color: var(--accent-color);
            color: var(--accent-color);
        }
        .wifi-form {
            display: flex;
            flex-direction: column;
            gap: 0.8rem;
        }
        .wifi-form input {
            background: rgba(0, 0, 0, 0.3);
            border: 1px solid rgba(255, 255, 255, 0.1);
            border-radius: 10px;
            padding: 0.7rem 1rem;
            color: var(--title-color);
            font-size: 0.95rem;
            outline: none;
            transition: border-color 0.3s;
            width: 100%;
        }
        .wifi-form input:focus {
            border-color: var(--accent-color);
        }
        .btn-wifi-save {
            background: linear-gradient(135deg, #2ecc71, #27ae60);
            color: #fff;
            border: none;
            border-radius: var(--border-radius);
            padding: 0.9rem;
            font-size: 1rem;
            font-weight: 600;
            cursor: pointer;
            transition: all 0.3s ease;
            box-shadow: 0 4px 15px rgba(46, 204, 113, 0.25);
        }
        .btn-wifi-save:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 20px rgba(46, 204, 113, 0.4);
        }
        .btn-wifi-save:disabled {
            background: #2c3540;
            color: #606f7b;
            cursor: not-allowed;
            box-shadow: none;
            transform: none;
        }
        .wifi-hint {
            font-size: 0.8rem;
            color: #858994;
            text-align: center;
        }
        /* --- 푸터 스타일 --- */
        footer {
            margin-top: 1rem;
            padding-top: 1.2rem;
            border-top: 1px solid rgba(255, 255, 255, 0.08);
            text-align: center;
            font-size: 0.82rem;
            color: #858994;
            display: flex;
            flex-direction: column;
            gap: 0.4rem;
        }
        footer a {
            color: var(--accent-color);
            text-decoration: none;
            transition: color 0.2s;
        }
        footer a:hover {
            color: var(--accent-hover);
            text-decoration: underline;
        }
    </style>
</head>
<body>
    <div class="container">
        <header>
            <h1>POV LED STICK</h1>
            <p class="subtitle">POV 스틱용 이미지를 업로드하고 리사이징합니다.</p>
        </header>

        <div class="mode-tabs" id="mode-tabs">
            <button class="mode-tab active" id="tab-image">이미지</button>
            <button class="mode-tab" id="tab-text">텍스트</button>
            <button class="mode-tab" id="tab-wifi">Wi-Fi</button>
        </div>

        <div class="drop-zone" id="drop-zone">
            <div class="drop-zone-icon">📁</div>
            <div class="drop-zone-text">이미지를 드래그하여 놓거나 클릭하여 선택하세요</div>
            <p class="subtitle" style="margin-top: 0.5rem;">세로 72픽셀로 자동 변환됩니다.</p>
            <input type="file" id="file-input" accept="image/*">
        </div>

        <div class="text-mode-panel" id="text-mode-panel">
            <div class="control-group">
                <label for="text-input">텍스트 입력</label>
                <textarea id="text-input" maxlength="200" rows="3"
                    placeholder="표시할 텍스트를 입력하세요 (한글 최대 20자, 영문 최대 40자, 여러 줄 가능)"></textarea>
                <span id="char-count" class="char-counter">0 / 40</span>
            </div>
            <div class="control-row">
                <div class="control-group control-half">
                    <label for="font-select">글꼴</label>
                    <div class="font-input-row">
                        <select id="font-select">
                            <option value="sans-serif">Sans-serif (고딕)</option>
                            <option value="serif">Serif (명조)</option>
                            <option value="monospace">Monospace (고정폭)</option>
                            <option value="Arial">Arial</option>
                            <option value="Helvetica">Helvetica</option>
                            <option value="Verdana">Verdana</option>
                            <option value="Georgia">Georgia</option>
                            <option value="Times New Roman">Times New Roman</option>
                            <option value="Courier New">Courier New</option>
                            <option value="Impact">Impact</option>
                        </select>
                        <button type="button" class="btn-load-font" id="btn-load-font" title="폰트 파일 불러오기">📁</button>
                    </div>
                    <input type="file" id="font-file-input" accept=".ttf,.otf,.woff,.woff2" style="display:none;">
                    <input type="hidden" id="custom-font-name" value="">
                </div>
            </div>
            <div class="control-group">
                <label for="text-color">글자 색상</label>
                <input type="color" id="text-color" value="#ffffff">
            </div>
            <div class="control-group">
                <label for="text-size">글자 크기: <span id="size-value">48</span>px</label>
                <input type="range" id="text-size" min="16" max="72" value="48" step="1">
            </div>
        </div>

        <div class="wifi-panel" id="wifi-panel">
            <div class="wifi-status-bar">
                <span class="wifi-mode-badge badge-ap" id="wifi-mode-badge">AP</span>
                <span class="wifi-ip-text" id="wifi-ip-text">로딩 중...</span>
                <button class="wifi-ip-copy" id="wifi-ip-copy" onclick="copyIp()">복사</button>
            </div>
            <div class="preview-title">Wi-Fi 설정</div>
            <div class="wifi-form">
                <div class="control-group">
                    <label for="wifi-scan-select">주변 Wi-Fi 목록</label>
                    <div class="font-input-row">
                        <select id="wifi-scan-select" onchange="onSelectWifi(this)">
                            <option value="">Wi-Fi를 검색 중이거나 선택하세요</option>
                        </select>
                        <button type="button" class="btn-load-font" id="btn-scan-wifi" onclick="scanWifi()" title="Wi-Fi 새로고침">🔄</button>
                    </div>
                </div>
                <div class="control-group">
                    <label for="wifi-ssid">SSID (공유기 이름)</label>
                    <input type="text" id="wifi-ssid" placeholder="공유기 SSID를 직접 입력하거나 위에서 선택하세요" autocomplete="off">
                </div>
                <div class="control-group">
                    <label for="wifi-pass">Password</label>
                    <input type="password" id="wifi-pass" placeholder="비밀번호를 입력하세요" autocomplete="off">
                </div>
                <button class="btn-wifi-save" id="wifi-save-btn" onclick="saveWifi()">저장 후 재부팅</button>
                <p class="wifi-hint">저장하면 ESP32가 자동으로 재부팅되어 공유기에 연결됩니다.</p>
                <div class="ip-upload-status" id="ip-upload-status"></div>
            </div>
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
            <div class="color-adjust-section" id="color-adjust-section">
                <div class="preview-title">색상 보정 (이미지 모드)</div>
                <div class="control-group">
                    <label for="sat-slider">채도: <span id="sat-value">1.7</span></label>
                    <input type="range" id="sat-slider" min="0" max="3.0" value="1.7" step="0.1">
                </div>
                <div class="control-group">
                    <label for="bright-slider">밝기: <span id="bright-value">0.3</span></label>
                    <input type="range" id="bright-slider" min="0.05" max="1.0" value="0.3" step="0.05">
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

        <footer>
            <div>GitHub: <a href="https://github.com/worynim/LED_POV_APA102" target="_blank" rel="noopener">github.com/worynim/LED_POV_APA102</a></div>
            <div>Email: <a href="mailto:arduinouno@kakao.com">arduinouno@kakao.com</a></div>
        </footer>
    </div>

    <script>
        const dropZone = document.getElementById('drop-zone');
        const fileInput = document.getElementById('file-input');
        const previewSection = document.getElementById('preview-section');
        const previewCanvas = document.getElementById('preview-canvas');
        const infoResolution = document.getElementById('info-resolution');
        const infoMemory = document.getElementById('info-memory');
        const colorAdjustSection = document.getElementById('color-adjust-section');
        const satSlider = document.getElementById('sat-slider');
        const satValue = document.getElementById('sat-value');
        const brightSlider = document.getElementById('bright-slider');
        const brightValue = document.getElementById('bright-value');
        const uploadBtn = document.getElementById('upload-btn');
        const progressContainer = document.getElementById('progress-container');
        const progressBar = document.getElementById('progress-bar');
        const statusMessage = document.getElementById('status-message');

        // 텍스트 모드 DOM 참조
        const modeTabs = document.getElementById('mode-tabs');
        const tabImage = document.getElementById('tab-image');
        const tabText = document.getElementById('tab-text');
        const textModePanel = document.getElementById('text-mode-panel');
        const textInput = document.getElementById('text-input');
        const charCount = document.getElementById('char-count');
        const fontSelect = document.getElementById('font-select');
        const textColorInput = document.getElementById('text-color');
        const textSizeSlider = document.getElementById('text-size');
        const sizeValueSpan = document.getElementById('size-value');
        const fontFileInput = document.getElementById('font-file-input');
        const btnLoadFont = document.getElementById('btn-load-font');
        const customFontName = document.getElementById('custom-font-name');

        let rawBuffer = null;
        let imgWidth = 0;
        let imgHeight = 72; // Target POV height
        let originalFileName = '';
        let currentImage = null; // 색상 보정 재적용용 최근 이미지

        // 텍스트 모드 상태
        let currentMode = 'image';
        let textRenderDebounce = null;

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

        // --- 텍스트 모드 이벤트 리스너 ---
        tabImage.addEventListener('click', () => switchMode('image'));
        tabText.addEventListener('click', () => switchMode('text'));

        textInput.addEventListener('input', updateCharCounter);

        fontSelect.addEventListener('change', () => {
            customFontName.value = '';  // 드롭다운 선택 시 커스텀 폰트 초기화
            if (textInput.value.trim()) {
                if (textRenderDebounce) clearTimeout(textRenderDebounce);
                textRenderDebounce = setTimeout(renderTextToCanvas, 150);
            }
        });

        textColorInput.addEventListener('input', () => {
            if (textInput.value.trim()) {
                if (textRenderDebounce) clearTimeout(textRenderDebounce);
                textRenderDebounce = setTimeout(renderTextToCanvas, 100);
            }
        });

        textSizeSlider.addEventListener('input', () => {
            sizeValueSpan.textContent = textSizeSlider.value;
            if (textInput.value.trim()) {
                if (textRenderDebounce) clearTimeout(textRenderDebounce);
                textRenderDebounce = setTimeout(renderTextToCanvas, 150);
            }
        });

        // --- 폰트 파일 불러오기 ---
        btnLoadFont.addEventListener('click', () => fontFileInput.click());

        fontFileInput.addEventListener('change', () => {
            const file = fontFileInput.files[0];
            if (!file) return;

            const fontName = file.name.replace(/\.[^.]+$/, ''); // 확장자 제거
            const reader = new FileReader();
            reader.onload = function(e) {
                const fontData = e.target.result;
                const mimeMap = {
                    'ttf': 'font/ttf', 'otf': 'font/otf',
                    'woff': 'font/woff', 'woff2': 'font/woff2'
                };
                const ext = file.name.split('.').pop().toLowerCase();
                const format = mimeMap[ext] || 'font/ttf';

                const fontFace = new FontFace(fontName, fontData, { style: 'normal', weight: 'bold' });
                fontFace.load().then(function(loadedFont) {
                    document.fonts.add(loadedFont);
                    customFontName.value = fontName;
                    showStatus("'" + fontName + "' 폰트 로드 완료", 'success');
                    if (textInput.value.trim()) {
                        if (textRenderDebounce) clearTimeout(textRenderDebounce);
                        textRenderDebounce = setTimeout(renderTextToCanvas, 200);
                    }
                }).catch(function(err) {
                    showStatus('폰트 로드 실패: ' + err.message, 'error');
                });
            };
            reader.readAsArrayBuffer(file);
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
                    currentImage = img;
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

                    // 2. 채도 증폭 & 밝기 감소 필터 적용 (웹 UI 슬라이더 값 사용)
                    const SATURATION_SCALE = parseFloat(satSlider.value);   // 채도 배율 (1.0이 원본)
                    const BRIGHTNESS_SCALE = parseFloat(brightSlider.value); // 밝기 배율 (1.0이 원본)

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
            colorAdjustSection.style.display = 'flex'; // 색상 보정 슬라이더 표시 (이미지 모드)
            // 파일명 입력 필드 표시 + 자동 채우기
            const nameInput = document.getElementById('image-name');
            const nameGroup = document.getElementById('name-input-group');
            nameInput.value = originalFileName;
            nameGroup.style.display = 'flex';
            showStatus('이미지 분석 완료. POV 스틱에 전송할 준비가 되었습니다.', 'success');
        }

        // --- 색상 보정 슬라이더 (이미지 모드): 값 변경 시 실시간 미리보기 재처리 ---
        satSlider.addEventListener('input', () => {
            satValue.textContent = (Math.round(parseFloat(satSlider.value) * 100) / 100).toString();
            if (currentImage) processImage(currentImage);
        });
        brightSlider.addEventListener('input', () => {
            brightValue.textContent = (Math.round(parseFloat(brightSlider.value) * 100) / 100).toString();
            if (currentImage) processImage(currentImage);
        });

        function showStatus(text, type) {
            statusMessage.textContent = text;
            statusMessage.className = type || '';
        }

        // --- 텍스트 모드 함수 ---
        function countCharacters(str) {
            let slots = 0;
            for (let i = 0; i < str.length; i++) {
                const code = str.charCodeAt(i);
                // 한글 음절 (U+AC00-U+D7AF), 자모 (U+1100-U+11FF, U+3130-U+318F)
                // CJK 통합 한자 (U+4E00-U+9FFF) — 모두 2칸 차지
                if ((code >= 0xAC00 && code <= 0xD7AF) ||
                    (code >= 0x1100 && code <= 0x11FF) ||
                    (code >= 0x3130 && code <= 0x318F) ||
                    (code >= 0x4E00 && code <= 0x9FFF)) {
                    slots += 2;
                } else {
                    slots += 1;  // ASCII, 숫자, 공백, 구두점
                }
            }
            return slots;
        }

        function updateCharCounter() {
            const slots = countCharacters(textInput.value);
            charCount.textContent = slots + ' / 40';
            charCount.classList.remove('warn', 'over');
            if (slots > 40) charCount.classList.add('over');
            else if (slots >= 32) charCount.classList.add('warn');
            // 디바운스된 미리보기 갱신
            if (textRenderDebounce) clearTimeout(textRenderDebounce);
            textRenderDebounce = setTimeout(renderTextToCanvas, 150);
        }

        function renderTextToCanvas() {
            const text = textInput.value.trim();
            if (!text) {
                rawBuffer = null;
                previewSection.style.display = 'none';
                colorAdjustSection.style.display = 'none';
                return;
            }

            const fontSize = parseInt(textSizeSlider.value);
            let fontFamily = customFontName.value || fontSelect.value;
            // 공백 포함된 폰트명은 따옴표로 감싸기
            if (fontFamily.includes(' ') && !fontFamily.startsWith("'")) {
                fontFamily = "'" + fontFamily + "'";
            }
            const textColor = textColorInput.value;

            // 1. 여러 줄로 분할하고 너비 측정
            const lines = text.split('\n');
            const measureCanvas = document.createElement('canvas');
            const mctx = measureCanvas.getContext('2d');
            mctx.font = fontSize + 'px ' + fontFamily;
            let maxWidth = 0;
            for (let i = 0; i < lines.length; i++) {
                const m = mctx.measureText(lines[i]);
                if (m.width > maxWidth) maxWidth = m.width;
            }
            let w = Math.ceil(maxWidth) + 4;  // 좌우 2px 여백
            w = Math.max(4, Math.min(300, w));

            const lineHeight = Math.round(fontSize * 1.15);
            const headroom = Math.round(fontSize * 0.3); // 상단 감지용 여유 공간
            const textAreaH = headroom + lineHeight * lines.length;

            // 2. 여유 공간 있는 임시 캔버스에 텍스트 렌더링
            const tempCanvas = document.createElement('canvas');
            tempCanvas.width = w;
            tempCanvas.height = textAreaH;
            const tctx = tempCanvas.getContext('2d');
            tctx.fillStyle = '#000000';
            tctx.fillRect(0, 0, w, textAreaH);  // 검은 배경
            tctx.font = fontSize + 'px ' + fontFamily;
            tctx.fillStyle = textColor;
            tctx.textBaseline = 'top';
            tctx.textAlign = 'center';
            for (let i = 0; i < lines.length; i++) {
                tctx.fillText(lines[i], w / 2, headroom + i * lineHeight);
            }

            // 3. 픽셀 스캔: 실제 글자가 시작되는 첫 y좌표 찾기
            const tempData = tctx.getImageData(0, 0, w, textAreaH).data;
            let textTop = 0;
            scanLoop:
            for (let y = 0; y < textAreaH; y++) {
                for (let x = 0; x < w; x++) {
                    const pi = (y * w + x) * 4;
                    if (tempData[pi] > 0 || tempData[pi + 1] > 0 || tempData[pi + 2] > 0) {
                        textTop = y;
                        break scanLoop;
                    }
                }
            }

            // 4. 72px 최종 캔버스에 크롭해서 그리기 (글자 상단이 y=0에 정확히 맞춰짐)
            const finalCanvas = document.createElement('canvas');
            finalCanvas.width = w;
            finalCanvas.height = 72;
            const fctx = finalCanvas.getContext('2d');
            fctx.fillStyle = '#000000';
            fctx.fillRect(0, 0, w, 72);
            fctx.drawImage(tempCanvas, 0, textTop, w, 72, 0, 0, w, 72);

            // 5. 픽셀 추출 → rawBuffer (텍스트 모드: 원본 색상 그대로)
            const imgData = fctx.getImageData(0, 0, w, 72);
            const data = imgData.data;
            const bodySize = w * 72 * 3;
            rawBuffer = new Uint8Array(2 + bodySize);
            rawBuffer[0] = (w >> 8) & 0xFF;
            rawBuffer[1] = w & 0xFF;

            let idx = 2;
            for (let x = 0; x < w; x++) {
                for (let y = 71; y >= 0; y--) {  // 하단 LED부터 (bottom-first)
                    const pi = (y * w + x) * 4;
                    rawBuffer[idx++] = data[pi];       // R
                    rawBuffer[idx++] = data[pi + 1];   // G
                    rawBuffer[idx++] = data[pi + 2];   // B
                }
            }

            // 6. 공유 미리보기 캔버스 업데이트
            imgWidth = w;
            imgHeight = 72;
            previewCanvas.width = w;
            previewCanvas.height = 72;
            const pctx = previewCanvas.getContext('2d');
            pctx.imageSmoothingEnabled = false;
            pctx.drawImage(finalCanvas, 0, 0);

            // 7. 정보 표시 및 파일명 자동 채우기
            infoResolution.textContent = w + ' x 72 px';
            infoMemory.textContent = Math.round(rawBuffer.length / 1024 * 10) / 10 + ' KB';
            previewSection.style.display = 'flex';
            colorAdjustSection.style.display = 'none'; // 텍스트 모드는 색상 보정 슬라이더 숨김
            const nameInput = document.getElementById('image-name');
            const nameGroup = document.getElementById('name-input-group');
            nameInput.value = text.replace(/[\\/:*?"<>|\n]/g, '_').substring(0, 18).trim() || 'text';
            nameGroup.style.display = 'flex';
        }

        function switchMode(mode) {
            if (currentMode === mode) return;
            currentMode = mode;
            tabImage.classList.toggle('active', mode === 'image');
            tabText.classList.toggle('active', mode === 'text');
            tabWifi.classList.toggle('active', mode === 'wifi');
            if (mode === 'image') {
                dropZone.style.display = '';
                textModePanel.style.display = 'none';
                wifiPanel.style.display = 'none';
                colorAdjustSection.style.display = 'flex'; // 이미지 로드 시에만 실제로 보임 (preview-section이 가려져 있으면 안 보임)
            } else if (mode === 'text') {
                dropZone.style.display = 'none';
                textModePanel.style.display = 'flex';
                wifiPanel.style.display = 'none';
                colorAdjustSection.style.display = 'none';
                if (textInput.value.trim()) renderTextToCanvas();
                else previewSection.style.display = 'none';
            } else if (mode === 'wifi') {
                dropZone.style.display = 'none';
                textModePanel.style.display = 'none';
                wifiPanel.style.display = 'flex';
                scanWifi();
            }
        }

        uploadBtn.addEventListener('click', () => {
            if (!rawBuffer) return;

            // 텍스트 모드: 업로드 전 유효성 검사
            if (currentMode === 'text') {
                if (!textInput.value.trim()) {
                    showStatus('텍스트를 입력해주세요.', 'error');
                    return;
                }
                if (countCharacters(textInput.value) > 40) {
                    showStatus('글자 수가 제한을 초과했습니다. (최대 40칸)', 'error');
                    return;
                }
            }

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

        // --- Wi-Fi 탭 이벤트 ---
        const tabWifi = document.getElementById('tab-wifi');
        const wifiPanel = document.getElementById('wifi-panel');
        tabWifi.addEventListener('click', () => switchMode('wifi'));

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

        // --- Wi-Fi 함수 ---
        function scanWifi() {
            const select = document.getElementById('wifi-scan-select');
            const btn = document.getElementById('btn-scan-wifi');
            if (select) select.innerHTML = '<option value="">Wi-Fi 검색 중...</option>';
            if (btn) btn.disabled = true;

            fetch('/wifi-scan')
                .then(r => r.json())
                .then(list => {
                    if (btn) btn.disabled = false;
                    if (!select) return;
                    if (!list || list.length === 0) {
                        select.innerHTML = '<option value="">검색된 Wi-Fi가 없습니다.</option>';
                        return;
                    }
                    select.innerHTML = '';  // SSID는 textContent로 삽입해 HTML 주입(XSS) 방지
                    const placeholder = document.createElement('option');
                    placeholder.value = '';
                    placeholder.textContent = '검색된 Wi-Fi를 선택하세요 (' + list.length + '개)';
                    select.appendChild(placeholder);
                    list.forEach(net => {
                        const opt = document.createElement('option');
                        opt.value = net.ssid;
                        opt.textContent = net.ssid + ' (' + net.rssi + 'dBm ' + (net.secure ? '🔒' : '🔓') + ')';
                        select.appendChild(opt);
                    });
                })
                .catch(err => {
                    if (btn) btn.disabled = false;
                    if (select) select.innerHTML = '<option value="">Wi-Fi 스캔 실패</option>';
                    console.error('Wi-Fi 스캔 실패:', err);
                });
        }

        function onSelectWifi(selectEl) {
            const val = selectEl.value;
            if (val) {
                document.getElementById('wifi-ssid').value = val;
                showStatus("SSID '" + val + "'이(가) 선택되었습니다.", 'success');
            }
        }

        function fetchWifiStatus() {
            fetch('/wifi-status')
                .then(r => r.json())
                .then(data => {
                    const badge = document.getElementById('wifi-mode-badge');
                    const ipText = document.getElementById('wifi-ip-text');
                    const ssidInput = document.getElementById('wifi-ssid');
                    badge.textContent = data.mode;
                    badge.className = 'wifi-mode-badge ' + (data.mode === 'STA' ? 'badge-sta' : 'badge-ap');
                    if (data.mode === 'STA') {
                        ipText.textContent = data.ip + ' (' + data.ssid + ')';
                    } else {
                        ipText.textContent = data.ip + ' (AP: ' + data.ssid + ')';
                    }
                    if (data.saved_ssid) ssidInput.value = data.saved_ssid;
                })
                .catch(err => console.error('Wi-Fi 상태 불러오기 실패:', err));
        }

        function copyIp() {
            const raw = document.getElementById('wifi-ip-text').textContent;
            const ip = raw.split(' ')[0];
            const done = () => showStatus('IP 주소가 복사되었습니다.', 'success');
            const fail = () => showStatus('복사 실패', 'error');
            // navigator.clipboard는 HTTPS/보안 컨텍스트에서만 동작 → http://192.168.x.x 는 폴백 필요
            if (navigator.clipboard && window.isSecureContext) {
                navigator.clipboard.writeText(ip).then(done).catch(fail);
            } else {
                const ta = document.createElement('textarea');
                ta.value = ip;
                ta.style.position = 'fixed';
                ta.style.opacity = '0';
                document.body.appendChild(ta);
                ta.select();
                ta.setSelectionRange(0, 99999);
                let ok = false;
                try { ok = document.execCommand('copy'); } catch (e) { ok = false; }
                document.body.removeChild(ta);
                ok ? done() : fail();
            }
        }

        function saveWifi() {
            const ssid = document.getElementById('wifi-ssid').value.trim();
            const pass = document.getElementById('wifi-pass').value;
            if (!ssid) { showStatus('SSID를 입력해주세요.', 'error'); return; }
            const btn = document.getElementById('wifi-save-btn');
            btn.disabled = true;
            btn.textContent = '저장 중...';
            fetch('/wifi-save', {
                method: 'POST',
                headers: { 'Content-Type': 'application/x-www-form-urlencoded' },
                body: 'ssid=' + encodeURIComponent(ssid) + '&pass=' + encodeURIComponent(pass)
            })
            .then(r => r.json())
            .then(data => {
                if (data.status === 'ok') {
                    btn.disabled = true;
                    btn.textContent = '재부팅 중...';
                    showStatus('저장 완료. 기기가 재부팅됩니다...', 'success');
                } else {
                    btn.disabled = false;
                    btn.textContent = '저장 후 재부팅';
                    showStatus('저장 실패: ' + (data.message || ''), 'error');
                }
            })
            .catch(() => {
                // 응답 후 즉시 재부팅되어 연결이 끊긴 경우가 대부분 → 재부팅 진행 중으로 안내
                btn.disabled = true;
                btn.textContent = '재부팅 중...';
                showStatus('저장되었습니다. 기기가 재부팅 중입니다...', 'success');
            });
        }

        // 페이지 로드 시 이미지 목록 불러오기
        window.addEventListener('load', () => {
            fetchImageList();
            fetchWifiStatus();
        });
    </script>
</body>
</html>
)rawliteral";

#endif

