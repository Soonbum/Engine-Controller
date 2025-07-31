# NVR_Control
NVR 엔진 제어 코드

1. [USB-Serial Software Development Kit' for Linux](http://www.cypress.com/documentation/software-and-drivers/usb-serial-software-development-kit)를 다운로드 받으세요.
2. 라즈베리파이에 다운로드 한 다음 설치합니다.
   * CyUSBSerial_SDK_Linux/linux/library 디렉토리에서 Makefile 파일을 찾습니다.
   * sudo make
   * sudo make install
   * sudo ldconfig
4. Qt Creator로 프로젝트를 생성합니다.
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

