#ifndef IENGINE_H
#define IENGINE_H

class IEngine
{
public:
    virtual ~IEngine() {}

    virtual bool projectorOnOff(bool projectorEnable) = 0;
    virtual int ledOnOff(bool ledEnable) = 0;
    virtual bool setLEDCurrent(int brightness) = 0;
    virtual int getLedCurrent() = 0;
};

#endif // IENGINE_H
