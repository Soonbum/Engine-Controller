#ifndef CLS_ENGINENQM_H
#define CLS_ENGINENQM_H

#include "IEngine.h"
#include "CyUSBSerial.h"
#include <QObject>

class Cls_EngineNQM : public QObject, public IEngine
{
    Q_OBJECT
public:
    explicit Cls_EngineNQM(QObject *parent = 0);
    ~Cls_EngineNQM();

    bool projectorOnOff(bool projectorEnable) override;
    int ledOnOff(bool ledEnable) override;
    bool setLEDCurrent(int brightness) override;
    int getLedCurrent() override;
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
