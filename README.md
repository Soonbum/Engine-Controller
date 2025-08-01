# Engine-Controller

## 엔진 제어 코드

1. [USB-Serial Software Development Kit' for Linux](http://www.cypress.com/documentation/software-and-drivers/usb-serial-software-development-kit)를 다운로드 받으세요.
2. 라즈베리파이에 다운로드 한 다음 설치합니다.
   * CyUSBSerial_SDK_Linux/linux/library 디렉토리에서 Makefile 파일을 찾습니다.
   * sudo make
   * sudo make install
   * sudo ldconfig
4. Qt 5 Creator로 프로젝트를 생성합니다.
5. 프로젝트에 다음 파일들을 포함시킵니다.
   * Headers: CyUSBSerial.h, Cls_EngineNVR.h, Cls_EngineNQM.h, ...
   * Sources: Cls_EngineNVR.cpp, Cls_EngineNQM.cpp, ...
6. NVR_Control.pro 파일에 SOURCES, HEADERS, LIBS를 지정합니다.
   ```pro
   ...
   SOURCES += main.cpp \
              Cls_EngineNVR.cpp \
              Cls_EngineNQM.cpp

   HEADERS += CyUSBSerial.h \
              Cls_EngineNVR.h \
              Cls_EngineNQM.h

   LIBS += -L/usr/local/lib
   LIBS += -lcyusbserial
   ```
7. 위 코드는 라즈베리파이 보드의 HDMI 포트가 엔진에 연결되어야 정상 작동합니다. HDMI 포트가 연결되어 있지 않으면 LED가 잠깐 켜졌다가 바로 꺼집니다.

## 라즈베리파이 보드에서 프로그램 항상 실행하기 (systemd 서비스 방식)

* 서비스 파일 생성: `sudo vim /etc/systemd/system/engine-controller.service`을 입력해서 다음과 같은 파일을 생성합니다.
  ```
  [Unit]
  Description=C++ UV Engine Control Service
  After=network.target

  [Service]
  Environment="DISPLAY=:0"
  ExecStart=/home/pi/build-NVR_Control-Desktop-Debug/Engine-Controller
  WorkingDirectory=/home/pi/Engine-Controller
  User=pi
  Restart=always
  RestartSec=10

  [Install]
  WantedBy=multi-user.target
  ```

* 서비스 등록 및 시작
  ```
  sudo systemctl daemon-reload
  sudo systemctl enable engine-controller.service
  sudo systemctl start engine-controller.service
  ```

* 서비스 확인: `sudo systemctl status engine-controller`라고 입력하면 서비스가 잘 실행되고 있는지 확인 가능합니다.

8. VNC를 이용해서 라즈베리파이를 제어하는 것을 권장합니다.

* PC: RealVNC Viewer > Raspberry Pi: VNC Server (계정: pi / 패스워드: raspberry 혹은 별도 설정한 패스워드)