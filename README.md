# NVR_Control

## NVR 엔진 제어 코드

1. [USB-Serial Software Development Kit' for Linux](http://www.cypress.com/documentation/software-and-drivers/usb-serial-software-development-kit)를 다운로드 받으세요.
2. 라즈베리파이에 다운로드 한 다음 설치합니다.
   * CyUSBSerial_SDK_Linux/linux/library 디렉토리에서 Makefile 파일을 찾습니다.
   * sudo make
   * sudo make install
   * sudo ldconfig
4. Qt 5 Creator로 프로젝트를 생성합니다.
5. 프로젝트에 다음 파일들을 포함시킵니다.
   * Headers: Cls_EngineNVR.h, CyUSBSerial.h
   * Sources: Cls_EngineNVR.cpp
6. NVR_Control.pro 파일에 SOURCES, HEADERS, LIBS를 지정합니다.
   ```pro
   ...
   SOURCES += main.cpp \
              Cls_EngineNVR.cpp

   HEADERS += Cls_EngineNVR.h \
              CyUSBSerial.h

   LIBS += -L/usr/local/lib
   LIBS += -lcyusbserial
   ```
7. 다음은 콘솔에서 작동하는 가장 기본적인 샘플 코드입니다. 컴파일시 오류가 발생하면 이슈는 자체적으로 해결하십시오.
   ```cpp
   #include <QCoreApplication>
   #include <stdio.h>
   #include <unistd.h>
   #include "Cls_EngineNVR.h"

   int main(int arcs, char *argv[])
   {
     QCoreApplication a(argc, argv);

     printf("NVR 엔진 제어를 시작합니다.\n");

     // NVR 엔진 클래스 객체 생성
     Cls_EngineNVR engine;

     // 프로젝터 켜기
     printf("프로젝터 On...\n");
     if (!engine.projectorOnOff(true)) {
       printf("프로젝터 켜기 실패!\n");
       return -1;
     }

     sleep(3);

     // LED 밝기 설정 (0~1023)
     int brightness = 100;
     engine.setLEDCurrent(brightness);
     printf("LED 밝기: %d...\n", brightness);

     // 현재 LED 밝기 값 확인
     int current_brightness = engine.getLedCurrent();
     printf("확인된 밝기 값: %d\n", current_brightness);

     // LED 켜기
     printf("LED On...\n");
     engine.ledOnOff(true);
     sleep(10);

     // LED 끄기
     printf("LED Off...\n");
     engine.ledOnOff(false);
     sleep(1);

     // 프로젝터 Off
     printf("프로젝터 Off...\n");
     engine.projectorOnOff(false);

     printf("프로그램 종료\n");

     return a.exec();
   }
   ```
8. 위 코드는 라즈베리파이 보드의 HDMI 포트가 엔진에 연결되어야 정상 작동합니다. HDMI 포트가 연결되어 있지 않으면 LED가 잠깐 켜졌다가 바로 꺼집니다.

## 라즈베리파이 보드에서 프로그램 자동 시작 활성화 (autostart 방식)

* 터미널에서 다음을 실행하고 내용을 넣으세요: `vi ~/.config/lxsession/LXDE-pi/autostart`
  - 다음과 같이 하면 test_window.py 파이썬 코드를 백그라운드로 실행합니다.
    ```
    @lxpanel --profile LXDE-pi
    @pcmanfm --desktop --profile LXDE-pi
    @/usr/bin/python3 /home/pi/test_window.py &
    ```

## 라즈베리파이 보드에서 프로그램 항상 실행하기 (systemd 서비스 방식)

* 서비스 파일 생성: `sudo vim /etc/systemd/system/engine-controller.service`을 입력해서 다음과 같은 파일을 생성합니다.
  ```
  [Unit]
  Description=C++ UV Engine Control Service
  After=network.target

  [Service]
  Environment="DISPLAY=:0"
  ExecStart=/home/pi/build-NVR_Control-Desktop-Debug/NVR_Control
  WorkingDirectory=/home/pi/build-NVR_Control-Desktop-Debug
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
