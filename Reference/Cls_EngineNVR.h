#ifndef CLS_ENGINENVR_H
#define CLS_ENGINENVR_H

#include "CyUSBSerial.h"

#include <QObject>

class Cls_EngineNVR : public QObject
{
    Q_OBJECT
public:
    explicit Cls_EngineNVR(QObject *parent = 0);
    ~Cls_EngineNVR();

//    bool first,stop;
//    bool mask;
//    int TotalLayer;
//    QString SliceType;

    bool imageMasking();
    bool gridMasking();
    bool projectorOnOff(bool projectorEnable);
    int ledOnOff(bool ledEnable);
    bool getLedOnOff();
    bool setLEDCurrent(int brightness);
    int getLedCurrent();
    int applyLedCurrent(int brightness);
    bool imageFlip(bool onOff);

    int getInteriorLight();
    int setInteriorLight(int sel, CY_HANDLE handle);

private:
    CY_RETURN_STATUS FlashBlockRead(int address, int len, char *buf);
    CY_RETURN_STATUS cySPIWaitForIdle(CY_HANDLE cyHandle);
    CY_HANDLE applyHandle;
    int count;

    void initCY();
    int FindDeviceAtSPIAndI2C(QString device);
    int batchFileWrite(int len, unsigned char buf[]);
    int batchFileRead(int len, unsigned char buf[]);
    void engineLog_write(QString value);

public slots:
    bool uvLED_Mask();
};

#endif // CLS_ENGINENVR0_H
