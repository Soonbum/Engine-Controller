#ifndef CLS_ENGINENQM_H
#define CLS_ENGINENQM_H

#include "CyUSBSerial.h"
#include <QObject>

class Cls_EngineNQM : public QObject
{
    Q_OBJECT
public:
    explicit Cls_EngineNQM(QObject *parent = 0);
    ~Cls_EngineNQM();

    bool projectorOnOff(bool projectorEnable);
    bool ledOnOff(bool ledEnable);
    bool setLEDCurrent(int brightness);
    int getLedCurrent();
    bool motorControl(bool rear_front, bool upDown, int step);
    int temperatureSensor(QString sort);
    bool imageFlip(bool onOff);
    int lightSensor();
    bool setFanSpeed(QString sort, int speed);
    int getASICREADY();
    bool showTestPatterns(QString sort);
    QString getAppVersion();
    QString getSequenceRevision();

public slots:
    bool getUniformityMask();

private:
    void initCY();
    int FindDeviceAtSPIAndI2C(QString device);
    bool read_GPIO1();
    CY_RETURN_STATUS FlashBlockRead(int address, int len, char *buf);
    CY_RETURN_STATUS cySPIWaitForIdle(CY_HANDLE cyHandle);
};

#endif // CLS_ENGINENQM_H
