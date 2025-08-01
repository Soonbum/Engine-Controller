#include "Cls_EngineNVR.h"

//#include "PopupWindow_Mgr/Dlg_Message.h"

#include <QCoreApplication>
#include <QFile>
#include <QDateTime>
#include <unistd.h>
#include <QDebug>

// 영옵틱스 1920x1080 엔진(cypress)

Cls_EngineNVR::Cls_EngineNVR(QObject *parent) : QObject(parent)
{
    count = 0;

    initCY();
}

Cls_EngineNVR::~Cls_EngineNVR()
{
    CyLibraryExit();
}

void Cls_EngineNVR::initCY()
{
    CyLibraryInit();
}

bool Cls_EngineNVR::projectorOnOff(bool projectorEnable)
{
    int deviceNumber = FindDeviceAtSPIAndI2C("SPI");

    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus == 5)
    {
        rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

        if(rStatus != CY_SUCCESS)
        {
            engineLog_write("projectONOFF open " + rStatus);
            printf("SPI Device open failed.\n");

            return false;
        }
    }

    UINT8 gpioNum = 2;
    UINT8 value = (UINT8)(projectorEnable ? 1 : 0);

    rStatus = CySetGpioValue(cyHandle, gpioNum, value);

    if(rStatus != CY_SUCCESS)
    {
        engineLog_write("projectONOFF value " + rStatus);

        printf("Error in doing gpio write \n");
        CyClose(cyHandle);

        return false;
    }

    CyClose(cyHandle);
    count = 0;

    return true;
}

int Cls_EngineNVR::ledOnOff(bool ledEnable)
{
    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");

    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        engineLog_write("open " + rStatus);
        printf("i2c Device open failed.\n");

        return -1;
    }

    CY_I2C_CONFIG cyI2CConfig;
    cyI2CConfig.frequency = 100000;
    cyI2CConfig.isMaster = 1;

    rStatus = CySetI2cConfig(cyHandle, &cyI2CConfig);

    if(rStatus != CY_SUCCESS)
    {
        printf("CySetI2cConfig returned failure code.\n");
        CyClose(cyHandle);

        return false;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1B;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[2];
    memset(wbuffer, 0, 2);
    int enableByte = 0;

    if(ledEnable == true) { enableByte = 0x07; } // R | G | B
    else if(ledEnable == false) { enableByte = 0; }

    wbuffer[0] = (unsigned char)0x52;
    wbuffer[1] = (unsigned char)enableByte;

    CY_DATA_BUFFER cyDatabufferWrite;
    cyDatabufferWrite.length = 2;
    cyDatabufferWrite.buffer = wbuffer;

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        engineLog_write("write " +rStatus);
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return 3;
    }

    CyClose(cyHandle);
    count = 0;

    return true;
}

bool Cls_EngineNVR::getLedOnOff()
{
    bool result;
    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");

    printf("Opening I2C device with device number %d...\n", deviceNumber);

    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return false;
    }

    CY_I2C_CONFIG cyI2CConfig;
    cyI2CConfig.frequency = 100000;
    cyI2CConfig.isMaster = 1;
    rStatus = CySetI2cConfig(cyHandle, &cyI2CConfig);

    if(rStatus != CY_SUCCESS)
    {
        printf("CySetI2cConfig returned failure code.\n");
        CyClose(cyHandle);

        return false;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1B;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[1], rbuffer[1];
    memset(wbuffer, 0, 1);
    memset(rbuffer, 0, 1);

    wbuffer[0] = (unsigned char)0x53;

    CY_DATA_BUFFER cyDatabufferWrite, cyDatabufferRead;
    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 1;
    cyDatabufferRead.buffer = rbuffer;
    cyDatabufferRead.length = 1;

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    // read
    rStatus = CyI2cReset(cyHandle, 0);  // write 1, read 0

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write reset \n");
        CyClose(cyHandle);

        return false;
    }

    rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    if((cyDatabufferRead.buffer[0] & 0x02) == 0x02) { result = true; }
    else { result = false; }

    CyClose(cyHandle);
    count = 0;

    return result;
}

bool Cls_EngineNVR::setLEDCurrent(int brightness)
{
    getLedCurrent();    // 영옵틱스의 추천 사항

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    printf("Opening SPI device with device number setLedCurrent %d...\n", deviceNumber);

    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return false;
    }

    CY_I2C_CONFIG cyI2CConfig;
    cyI2CConfig.frequency = 100000;
    cyI2CConfig.isMaster = 1;
    rStatus = CySetI2cConfig(cyHandle, &cyI2CConfig);

    if(rStatus != CY_SUCCESS)
    {
        printf("CySetI2cConfig returned failure code.\n");
        CyClose(cyHandle);

        return false;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1B;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[7];
    memset(wbuffer, 0, 7);

    wbuffer[0] = (unsigned char)0x54;
    wbuffer[1] = (unsigned char)(brightness & 0xFF);    // R
    wbuffer[2] = (unsigned char)((brightness >> 8) & 0xFF);
    wbuffer[3] = (unsigned char)(brightness & 0xFF);    // G
    wbuffer[4] = (unsigned char)((brightness >> 8) & 0xFF);
    wbuffer[5] = (unsigned char)(brightness & 0xFF);    // B
    wbuffer[6] = (unsigned char)((brightness >> 8) & 0xFF);

    CY_DATA_BUFFER cyDatabufferWrite;
    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 7;

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    CyClose(cyHandle);

    return true;
}

int Cls_EngineNVR::applyLedCurrent(int brightness)
{
    CY_RETURN_STATUS rStatus;
    int len = 256;
    unsigned char batchFileBuf[len];
    memset(batchFileBuf, 0, 256);

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    int interfaceNum = 0;
    printf("Opening SPI device with device number applyLedCurrent %d...\n", deviceNumber);
    rStatus = CyOpen(deviceNumber, interfaceNum, &applyHandle);

    if(rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            applyLedCurrent(brightness);
        }

        printf("i2c Device open failed.\n");
        return false;
    }

    int ret = batchFileRead(len, batchFileBuf);

    if(ret < 0)
    {
        printf("batchFileRead() fail \n");
        return ret;
    }

    batchFileBuf[42] = (unsigned char)0x00; // R
    batchFileBuf[43] = (unsigned char)0x00;
    batchFileBuf[44] = (unsigned char)(brightness & 0xFF);  // G
    batchFileBuf[45] = (unsigned char)((brightness >> 8) & 0XFF);
    batchFileBuf[46] = (unsigned char)0x00; // B
    batchFileBuf[47] = (unsigned char)0x00;

    ret = batchFileWrite(len, batchFileBuf);

    if(ret < 0)
    {
        printf("batchFileWrite() fail \n");
        return ret;
    }

    return 0;
}

int Cls_EngineNVR::batchFileRead(int len, unsigned char buf[])
{
    CY_RETURN_STATUS rStatus;

    int blockSize = 0x0100;
    int cWriteSize = 5;
    int cReadSize = blockSize;
    unsigned char sendBuf[cWriteSize];
    unsigned char recvBuf[cReadSize];
    memset(sendBuf, 0, cWriteSize);
    memset(recvBuf, 0, cReadSize);

    int ret, first = 1, address = 0;

    sendBuf[0] = 0xDE;
    sendBuf[1] = 0x30;
    sendBuf[2] = 0x00;
    sendBuf[3] = 0x00;
    sendBuf[4] = 0x00;
    cWriteSize = 5;

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1B;
    i2cDataConfig.isNakBit = true;

    CY_DATA_BUFFER cyDatabufferWrite,cyDatabufferRead;
    cyDatabufferWrite.buffer = sendBuf;
    cyDatabufferWrite.length = cWriteSize;
    cyDatabufferRead.buffer = recvBuf;
    cyDatabufferRead.length = cReadSize;

    rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
    ret = (-1) * rStatus;

    if(ret < 0)
    {
        printf("CyI2cWrite() fail \n");
        CyClose(applyHandle);

        return ret;
    }

    while(len > 0)
    {
        if(len > blockSize) { cReadSize = blockSize; }
        else { cReadSize = len; }

        sendBuf[0] = 0xDF;
        sendBuf[1] = (unsigned char)(cReadSize & 0xff);
        sendBuf[2] = (unsigned char)((cReadSize >> 8) & 0xff);

        cWriteSize = 3;
        cyDatabufferWrite.buffer = sendBuf;
        cyDatabufferWrite.length = cWriteSize;

        rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cWrite(sencond) fail \n");
            CyClose(applyHandle);

            return ret;
        }

        if(first == 1)
        {
            sendBuf[0] = 0xE3;
            first = 0;
        }
        else { sendBuf[0] = 0xE4; }

        cWriteSize = 1;
        cyDatabufferWrite.buffer = sendBuf;
        cyDatabufferWrite.length = cWriteSize;

        rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cWrite(third) fail \n");
            CyClose(applyHandle);

            return ret;
        }

        rStatus = CyI2cReset(applyHandle, 0);   // write 1, read 0

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write reset \n");
            CyClose(applyHandle);

            return false;
        }

        rStatus = CyI2cRead(applyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cRead fail \n");
            CyClose(applyHandle);

            return ret;
        }

        for(int i = 0; i < cReadSize; i++)
        {
            buf[address + i] = recvBuf[i];
        }

        address += blockSize;

        if(len > blockSize) { len -=blockSize; }
        else { len = 0; }
    }

    return 0;
}

int Cls_EngineNVR::batchFileWrite(int len, unsigned char buf[])
{
    CY_RETURN_STATUS rStatus;

    int blockSize = 0x0100;
    int cWriteSize = blockSize + 1;
    int cReadSize = blockSize;
    unsigned char sendBuf[cWriteSize];
    unsigned char recvBuf[cReadSize];
    memset(sendBuf, 0, cWriteSize);
    memset(recvBuf, 0, cReadSize);
    int ret, first = 1, address = 0;

    sendBuf[0] = 0xDE;
    sendBuf[1] = 0x30;
    sendBuf[2] = 0x00;
    sendBuf[3] = 0x00;
    sendBuf[4] = 0x00;
    cWriteSize = 5;

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1B;
    i2cDataConfig.isNakBit = true;

    CY_DATA_BUFFER cyDatabufferWrite,cyDatabufferRead;
    cyDatabufferWrite.buffer = sendBuf;
    cyDatabufferWrite.length = cWriteSize;
    cyDatabufferRead.buffer = recvBuf;
    cyDatabufferRead.length = cReadSize;

    rStatus = CyI2cReset(applyHandle, 1);   // write 1, read 0

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write reset \n");
        CyClose(applyHandle);

        return false;
    }

    rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
    ret = (-1) * rStatus;

    if(ret < 0)
    {
        printf("CyI2cWrite(1) fail \n");
        CyClose(applyHandle);

        return ret;
    }

    sendBuf[0] = 0xE0;
    sendBuf[1] = 0xAA;
    sendBuf[2] = 0xBB;
    sendBuf[3] = 0xCC;
    sendBuf[4] = 0xDD;
    cWriteSize = 5;
    cyDatabufferWrite.buffer = sendBuf;
    cyDatabufferWrite.length = cWriteSize;

    rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
    ret = (-1) * rStatus;

    if(ret < 0)
    {
        printf("CyI2cWrite(2) fail \n");
        CyClose(applyHandle);

        return ret;
    }

    do
    {
        sendBuf[0] = 0xD0;
        cWriteSize = 1;
        cyDatabufferWrite.buffer = sendBuf;
        cyDatabufferWrite.length = cWriteSize;

        rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cWrite(3) fail \n");
            CyClose(applyHandle);
            return ret;
        }

        cReadSize = 1;
        rStatus = CyI2cReset(applyHandle, 0);   // write 1, read 0

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write reset \n");
            CyClose(applyHandle);

            return false;
        }

        cyDatabufferRead.buffer = recvBuf;
        cyDatabufferRead.length = cReadSize;
        rStatus = CyI2cRead(applyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cRead fail \n");
            CyClose(applyHandle);

            return ret;
        }
    }
    while((recvBuf[0] & 0x20) != 0);

    while(len > 0)
    {

        if(len > blockSize) { cReadSize = blockSize; }
        else { cReadSize = len; }

        sendBuf[0] = 0xDF;
        sendBuf[1] = (unsigned char)(cReadSize & 0xff);
        sendBuf[2] = (unsigned char)((cReadSize >> 8) & 0xff);
        cWriteSize = 3;

        rStatus = CyI2cReset(applyHandle, 1);   // write 1, read 0

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write reset \n");
            CyClose(applyHandle);

            return false;
        }

        cyDatabufferWrite.buffer = sendBuf;
        cyDatabufferWrite.length = cWriteSize;
        rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cWrite(4) fail \n");
            CyClose(applyHandle);

            return ret;
        }

        if(first == 1)
        {
            sendBuf[0] = 0xE1;
            first = 0;
        }
        else { sendBuf[0] = 0xE2; }

        cWriteSize = 1;

        for(int i = 0; i < cReadSize; i++)
        {
            sendBuf[i + cWriteSize] = buf[address + i];
        }

        cyDatabufferWrite.buffer = sendBuf;
        cyDatabufferWrite.length = cWriteSize;

        rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cWrite(5) fail \n");
            CyClose(applyHandle);

            return ret;
        }

        sendBuf[0] = 0xD0;
        cWriteSize = 1;
        cyDatabufferWrite.buffer = sendBuf;
        cyDatabufferWrite.length = cWriteSize;
        rStatus = CyI2cWrite(applyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cWrite(6) fail \n");
            CyClose(applyHandle);

            return ret;
        }

        rStatus = CyI2cReset(applyHandle, 0);   // write 1, read 0

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write reset \n");
            CyClose(applyHandle);

            return false;
        }

        cReadSize = 1;
        cyDatabufferRead.buffer = recvBuf;
        cyDatabufferRead.length = cReadSize;
        rStatus = CyI2cRead(applyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);
        ret = (-1) * rStatus;

        if(ret < 0)
        {
            printf("CyI2cRead fail \n");
            CyClose(applyHandle);

            return ret;
        }

        if((recvBuf[0] & 0x40) != 0) { return -1; } // write flash error

        address += blockSize;

        if(len > blockSize) { len -=blockSize; }
        else { len = 0; }
    }

    CyClose(applyHandle);

    return 0;
}

int Cls_EngineNVR::getLedCurrent()
{
    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return false;
    }

    CY_I2C_CONFIG cyI2CConfig;
    cyI2CConfig.frequency = 100000;
    cyI2CConfig.isMaster = 1;
    rStatus = CySetI2cConfig(cyHandle, &cyI2CConfig);

    if(rStatus != CY_SUCCESS)
    {
        printf("CySetI2cConfig returned failure code.\n");
        CyClose(cyHandle);

        return false;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1B;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[1],rbuffer[6];
    memset(wbuffer, 0, 1);
    memset(rbuffer, 0, 6);

    wbuffer[0] = (unsigned char)0x55;

    CY_DATA_BUFFER cyDatabufferWrite, cyDatabufferRead;
    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 1;
    cyDatabufferRead.buffer = rbuffer;
    cyDatabufferRead.length = 6;

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    // read
    rStatus = CyI2cReset(cyHandle, 0);  // write 1, read 0

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write reset \n");
        CyClose(cyHandle);

        return false;
    }

    rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    int green = (cyDatabufferRead.buffer[3] << 8) | cyDatabufferRead.buffer[2];

    CyClose(cyHandle);
    count = 0;

    return green;
}

bool Cls_EngineNVR::uvLED_Mask()
{
    int addr = 0x10000;
    char header_buf[] = { 0x00, 0x00, 0x00, 0x00,
                          0x00, 0x00, 0x00, 0x00 }; // 4bytes file length and 4bytes file check sum
    char *png_buf = NULL;
    int status;
    UINT32 png_len = 0;
    UINT32 png_cksum = 0;

    status = FlashBlockRead(addr, 8, header_buf);

    if(status != CY_SUCCESS)
    {
        printf("\nFailed to Read.");
        return false;
    }

    png_len = (header_buf[0] & 0xff) |
              ((header_buf[1] << 8) & 0xff00) |
              ((header_buf[2] << 16) & 0xff0000) |
              ((header_buf[3] << 24) & 0xff000000);

    png_cksum = (header_buf[4] & 0xff) |
                ((header_buf[5] << 8) & 0xff00) |
                ((header_buf[6] << 16) & 0xff0000) |
                ((header_buf[7] << 24) & 0xff000000);

    png_buf = new char[png_len];
    status = FlashBlockRead(addr + 8, png_len, png_buf);

    if(status != CY_SUCCESS)
    {
        printf("\nFailed to Read.");
        return false;
    }

    UINT32 cksum = 0;

    for(UINT32 i = 0; i < png_len; i++)
    {
        cksum += png_buf[i] & 0xff;
    }

    if(cksum != png_cksum)
    {
        printf("\nFile check sum is NG.");
        return false;
    }
    else { printf("\nFile check sum is OK."); }

    FILE *pFile;
    QString Path = QCoreApplication::applicationDirPath() + "/readflash.png" ;
    const char *flashFile = Path.toLatin1().data();

    pFile = fopen(flashFile, "wb");

    if(NULL == pFile)
    {
        printf("file open failure!\n");
        return false;
    }
    else { fwrite(png_buf, 1, png_len, pFile); }

    fclose(pFile);
    delete[] png_buf;

    return true;
}

CY_RETURN_STATUS Cls_EngineNVR::FlashBlockRead(int address, int len, char *buf)
{
    int BlockSize = 0x1000; // Block read 4KB size
    unsigned char wbuffer[0x1004], rbuffer[0x1004]; // array size is equal to BlockSize+4
    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    CY_DATA_BUFFER cyDatabufferWrite, cyDatabufferRead;
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 1;   //1? 0?
    int cWriteSize, cReadSize;
    int str_addr = address;

    printf("Opening SPI device with device number FlashBlockRead %d...\n", deviceNumber);

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("SPI Device open failed.\n");
        return rStatus;
    }

    while(len > 0)
    {
        printf("Read data from start address :0x%X \n", address);
        wbuffer[0] = 0x03;
        wbuffer[1] = (address >> 16 & 0xff);
        wbuffer[2] = (address >> 8 & 0xff);
        wbuffer[3] = (address & 0xff);
        cWriteSize = 4;

        if(len > BlockSize) { cReadSize = BlockSize; }
        else { cReadSize = len; }

        cyDatabufferWrite.buffer = wbuffer;
        cyDatabufferWrite.length = 4 + cReadSize; // 4 bytes command + 256 bytes page size
        cyDatabufferRead.buffer = rbuffer;
        cyDatabufferRead.length = 4 + cReadSize;
        rStatus = CySpiReadWrite (cyHandle, &cyDatabufferRead,&cyDatabufferWrite, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing SPI data write :0x%X \n", cyDatabufferWrite.transferCount);
            CyClose(cyHandle);

            return rStatus;
        }

        for(int i = 0; i < cReadSize; i++)
        {
            buf[address - str_addr + i] = rbuffer[cWriteSize + i];
        }

        rStatus = cySPIWaitForIdle(cyHandle);

        if(rStatus)
        {
            printf("Error in Waiting for Flash active state:0x%X \n", rStatus);
            CyClose(cyHandle);

            return rStatus;
        }

        address += BlockSize;

        if(len > BlockSize) { len -= BlockSize; }
        else { len = 0; }
    }

    printf("Closing SPI device...\n");
    rStatus = CyClose (cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("SPI Device close failed.\n");
        return rStatus;
    }

    printf("Flash Read Done ...");

    return CY_SUCCESS;
}

int Cls_EngineNVR::FindDeviceAtSPIAndI2C(QString device)
{
    CY_RETURN_STATUS rStatus;
    CY_DEVICE_INFO deviceInfo;
    UINT8 cyNumDevices;

    rStatus = CyGetListofDevices(&cyNumDevices);

    if(rStatus != CY_SUCCESS)
    {
        printf("Device open failed.\n");
        return rStatus;
    }

    for(UINT8 i = 0; i <= cyNumDevices; i++)
    {
        rStatus = CyGetDeviceInfo(i, &deviceInfo);

        if(device == "I2C")
        {
            if(deviceInfo.deviceType[0] == CY_TYPE_I2C) { return i; }
        }
        else if(device == "SPI")
        {
            if(deviceInfo.deviceType[1] == CY_TYPE_SPI)
            {
                return i;
            }
        }
    }

    return 10;
}

CY_RETURN_STATUS Cls_EngineNVR::cySPIWaitForIdle(CY_HANDLE cyHandle)
{
    char rd_data[2], wr_data[2];
    CY_DATA_BUFFER writeBuf, readBuf;
    int timeout = 0xFFFF;
    CY_RETURN_STATUS status;

    printf("\nSending SPI Status query command to device...");
    writeBuf.length = 2;
    writeBuf.buffer = (unsigned char *)wr_data;
    readBuf.length = 2;
    readBuf.buffer = (unsigned char *)rd_data;

    do {
        wr_data[0] = 0x05; /* Get SPI status */
        status = CySpiReadWrite(cyHandle, &readBuf, &writeBuf, 5000);

        if(status != CY_SUCCESS)
        {
            printf("\nFailed to send SPI status query command to device.");
            CyClose(cyHandle);

            break;
        }

        timeout--;

        if(timeout == 0)
        {
            printf("\nMaximum retries completed while checking SPI status, returning with error code.");
            status = CY_ERROR_IO_TIMEOUT;

            return status;
        }

    } while(rd_data[1] & 0x03); // Check SPI Status

    printf("\nSPI is now in idle state and ready for receiving additional data commands.");

    return status;
}

bool Cls_EngineNVR::imageFlip(bool onOff)
{
    getLedCurrent(); //영옵틱스의 추천 사항

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");

    printf("Opening I2C device with device number %d...\n", deviceNumber);

    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return false;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1B;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[2];
    memset(wbuffer, 0, 2);
    wbuffer[0] = (unsigned char)0x14;

    if(onOff) { wbuffer[1] = (unsigned char)0x02; }
    else { wbuffer[1] = (unsigned char)0x00; }

    CY_DATA_BUFFER cyDatabufferWrite;
    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 2;
    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    CyClose(cyHandle);

    return true;
}


// 내부조도계 (테스트 필요)
int Cls_EngineNVR::getInteriorLight()
{
    //Dlg_Message dlg;

    //getLedOnOff
    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");

    printf("Opening I2C device with device number %d...\n", deviceNumber);

    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;
    int Light_Sensor = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if (rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            getInteriorLight();
        }

        //dlg.setMessage("English", ("i2c Device open failed.\n"), 0);
        printf("i2c Device open failed.\n");

        return false;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig2;

    i2cDataConfig2.isStopBit = true;
    i2cDataConfig2.slaveAddress = 0x39;
    i2cDataConfig2.isNakBit = true;

    unsigned char wbuffer[1],rbuffer[2];

    memset(wbuffer, 0, 1);
    memset(rbuffer, 0, 2);
    wbuffer[0] = (unsigned char)0xAC;

    CY_DATA_BUFFER cyDatabufferWrite, cyDatabufferRead;

    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 1;
    cyDatabufferRead.buffer = rbuffer;
    cyDatabufferRead.length = 2;

    int ret = setInteriorLight(1, cyHandle);

    if(ret != 0)
    {
        //dlg.setMessage("English", ("Light Error in doing light sensor power on \n"),0);
        CyClose (cyHandle);
        return false;
    }

    //write
    rStatus = CyI2cReset(cyHandle, 1);//write 1, read 0

    if (rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            getInteriorLight();
        }

        //dlg.setMessage("English", ("Light Error in doing i2c write reset \n"),0);
        printf ("Error in doing i2c write reset \n");
        CyClose (cyHandle);

        return false;
    }

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig2, &cyDatabufferWrite, 5000);

    if (rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            getInteriorLight();
        }

        //dlg.setMessage("English", ("Light Error in doing i2c write \n"),0);
        printf ("Error in doing i2c write \n");
        CyClose (cyHandle);

        return false;
    }

    //read
    rStatus = CyI2cReset(cyHandle, 0);//write 1, read 0

    if (rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            getInteriorLight();
        }

        //dlg.setMessage("English", ("Light Error in doing i2c read reset \n"),0);
        printf ("Error in doing i2c write reset \n");
        CyClose (cyHandle);

        return false;
    }

    rStatus = CyI2cRead(cyHandle, &i2cDataConfig2, &cyDatabufferRead, 5000);

    if (rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            getInteriorLight();
        }

        //dlg.setMessage("English", ("Light Error in doing i2c read \n"),0);
        printf ("Error in doing i2c write \n");
        CyClose (cyHandle);

        return false;
    }
    Light_Sensor = (cyDatabufferRead.buffer[1] << 8) | cyDatabufferRead.buffer[0];

    ret = setInteriorLight(0, cyHandle);

    if(ret != 0)
    {
        //dlg.setMessage("English", ("Light Error in doing light sensor power off \n"),0);
        CyClose (cyHandle);

        return false;
    }
    CyClose(cyHandle);
    count = 0;

    return Light_Sensor; // 리턴값은 밝기값
}

int Cls_EngineNVR::setInteriorLight(int sel, CY_HANDLE handle)
{
    // 빛 세기 측정 센서 파워 on/off

    //Dlg_Message dlg;
    CY_RETURN_STATUS rStatus;
    CY_I2C_DATA_CONFIG i2cDataConfig;

    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x39;
    i2cDataConfig.isNakBit = true;

    unsigned char wbuffer[2];

    memset(wbuffer, 0, 2);
    wbuffer[0] = (unsigned char)0xA0;

    if(sel == 1) { wbuffer[1] = (unsigned char)0x03; }
    else { wbuffer[1] = (unsigned char)0x00; }

    CY_DATA_BUFFER cyDatabufferWrite;

    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 2;
    rStatus = CyI2cReset(handle, 1); // write 1, read 0

    if (rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            setInteriorLight(sel, handle);
        }

        //dlg.setMessage("English", ("Light power sensor Error in doing i2c write reset \n"), 0);
        printf("Error in doing i2c write reset \n");
        CyClose(handle);

        return 1;
    }

    rStatus = CyI2cWrite(handle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if (rStatus != CY_SUCCESS)
    {
        count++;

        if(count < 3)
        {
            projectorOnOff(true);
            setInteriorLight(sel, handle);
        }

        //dlg.setMessage("English", ("Light power sensor Error in doing i2c write \n"), 0);
        printf ("Error in doing i2c write \n");
        CyClose (handle);

        return 1;
    }

    if(sel == 1) { usleep(500000); }

    return 0;
}
// 내부조도계 (테스트 필요)_end

void Cls_EngineNVR::engineLog_write(QString value)
{
    QFile log;
    log.setFileName("engineLog.txt");
    log.open(QIODevice::Append | QIODevice::Text);

    QString text = QDateTime::currentDateTime().toString("dd.MM.yyyy hh:mm:ss ") + value + "\n";
    QTextStream out(&log);

    out.setCodec("UTF-8");
    out << text;
    log.close();
}
