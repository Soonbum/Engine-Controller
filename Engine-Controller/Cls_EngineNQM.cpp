#include "Cls_EngineNQM.h"

//#include "PopupWindow_Mgr/Dlg_Message.h"

#include <unistd.h>
#include <QCoreApplication>

// 영옵틱스 3840엔진(pdf확인)
Cls_EngineNQM::Cls_EngineNQM(QObject *parent) : QObject(parent)
{
    initCY();
}

Cls_EngineNQM::~Cls_EngineNQM()
{
    CyLibraryExit();
}

void Cls_EngineNQM::initCY()
{
    CyLibraryInit();
}

bool Cls_EngineNQM::projectorOnOff(bool projectorEnable)
{
    /*Set GPIO-0 (RESETZ) to 1 to turn on the projector (controller), then we have to
    keep polling GPIO-15 (ASIC_READY) until its value is turned into 1, before taking
    any further access to the GPIO/I2C interface. The processing time normally takes
    about 14 seconds.
    Clear GPIO-0 (RESETZ) to 0 to turn off the projector (controller). It is not equal to
    power off the projector, and we have to clear it off before shutdown, to make sure
    that the DMD is safely parked.*/

    int deviceNumber = FindDeviceAtSPIAndI2C("SPI");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("SPI Device open failed.\n");
        return false;
    }

    UINT8 gpioNum = 0; // RESETZ
    UINT8 value = (UINT8)(projectorEnable ? 1 : 0);

    rStatus = CySetGpioValue(cyHandle, gpioNum, value);

    if(rStatus != CY_SUCCESS)
    {
        printf ("Error in doing gpio write \n");
        CyClose (cyHandle);

        return false;
    }

    CyClose(cyHandle);

    return true;
}

int Cls_EngineNQM::getASICREADY()
{
    int deviceNumber = FindDeviceAtSPIAndI2C("SPI");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS) { return false; }

    UINT8 gpioNum = 15; // ASIC_READY
    UINT8 value = 1;

    rStatus = CyGetGpioValue(cyHandle, gpioNum, &value);

    if(rStatus != CY_SUCCESS)
    {
        CyClose (cyHandle);
        return false;
    }

    CyClose(cyHandle);

    return value;
}

int Cls_EngineNQM::ledOnOff(bool ledEnable)
{
    /*Set GPIO-14 (LED_ON_OFF) to 1 to turn on the UV LED
    clear it to 0 to turn off LED.*/

    int deviceNumber = FindDeviceAtSPIAndI2C("SPI");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("SPI Device open failed.\n");
        return 0;
    }

    UINT8 gpioNum = 14; // LED_ON_OFF
    UINT8 value = (UINT8)(ledEnable ? 1 : 0);

    rStatus = CySetGpioValue(cyHandle, gpioNum, value);

    if(rStatus != CY_SUCCESS)
    {
        printf ("Error in doing gpio write \n");
        CyClose(cyHandle);

        return 0;
    }

    CyClose(cyHandle);

    return 1;
}

bool Cls_EngineNQM::setLEDCurrent(int brightness) // LED DAC
{
    /*Note: The Brightness of NQM UV Light Engine is proportional to the LED
    Current, which is represented by the LED DAC value of 0 and 50 ~
    1000, mapping to 0 and 1.25 ~ 25.0 (Amp). The LED DAC value is
    calculated by the formula:
                  dac = 1000 * (LED Current value) / 25.0
    For example, to set the LED Current value into 15 Amp, you have to
    set the DAC value into 1000*15.0/25.0 = 600 (0x0258).*/

    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return false;
    }

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

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[3];
    memset(wbuffer, 0, 3);

    wbuffer[0] = (unsigned char)0xD1;
    wbuffer[1] = (unsigned char)((brightness >> 8) & 0xFF);
    wbuffer[2] = (unsigned char)(brightness & 0xFF);

    CY_DATA_BUFFER cyDatabufferWrite;
    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 3;

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf ("Error in doing i2c write \n");
        CyClose (cyHandle);

        return false;
    }

    CyClose(cyHandle);

    return true;
}

int Cls_EngineNQM::getLedCurrent()
{
    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return false;
    }

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return -1;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[2],rbuffer[2];
    memset(wbuffer, 0, 2);
    memset(rbuffer, 0, 2);

    wbuffer[0] = (unsigned char)0x15;
    wbuffer[1] = (unsigned char)0xD1;

    CY_DATA_BUFFER cyDatabufferWrite, cyDatabufferRead;
    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 2;
    cyDatabufferRead.buffer = rbuffer;
    cyDatabufferRead.length = 2;

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf ("Error in doing i2c write \n");
        CyClose (cyHandle);

        return -1;
    }

    usleep(100000);
    rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf ("Error in doing i2c write \n");
        CyClose (cyHandle);

        return -1;
    }

    int DAC = (cyDatabufferRead.buffer[0] << 8) + cyDatabufferRead.buffer[1];

    CyClose(cyHandle);

    return DAC;
}

bool Cls_EngineNQM::motorControl(bool rear_front, bool upDown, int step)
{
    // MOTOR = 0 : front group motor, rear_front = 1 : rear group motor
    // upDown = 0 : up, 1 : down

    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return false;
    }

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

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;
    unsigned char direction[2], stepSize[5];
    memset(direction, 0, 2); memset(stepSize, 0, 5);

    if(rear_front) // 줌
    {
        direction[0] = (unsigned char)0xB5;
        direction[1] = (unsigned char)upDown;
        stepSize[0] = (unsigned char)0xB6;
        stepSize[1] = (unsigned char)(step & 0xFF);
        stepSize[2] = (unsigned char)((step >> 8) & 0xFF);
        stepSize[3] = (unsigned char)0x32;
        stepSize[4] = (unsigned char)0x00;
    }
    else // 초점
    {
        direction[0] = (unsigned char)0xBA;
        direction[1] = (unsigned char)upDown;
        stepSize[0] = (unsigned char)0xBB;
        stepSize[1] = (unsigned char)(step & 0xFF);
        stepSize[2] = (unsigned char)((step >> 8) & 0xFF);
        stepSize[3] = (unsigned char)0x64;
        stepSize[4] = (unsigned char)0x00;
    }

    CY_DATA_BUFFER cyDatabufferWrite;
    cyDatabufferWrite.length = 2;
    cyDatabufferWrite.buffer = direction;
    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    CY_DATA_BUFFER cyDatabufferWrite2;
    cyDatabufferWrite2.length = 5;
    cyDatabufferWrite2.buffer = stepSize;
    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite2, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return false;
    }

    CyClose(cyHandle);

    return true;
}

bool Cls_EngineNQM::setFanSpeed(QString sort, int speed)
{
    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return false;
    }

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

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;

    unsigned char input[2];
    memset(input, 0, 2);
    CY_DATA_BUFFER cyDatabufferWrite;

    if(sort == "DMD") { input[0] = (unsigned char)0xEB; }
    else if(sort == "LED_FAN1") { input[0] = (unsigned char)0xEC; }
    else if(sort == "LED_FAN2") { input[0] = (unsigned char)0xED; }
    else { return false; }

    input[1] = (unsigned char)speed;
    cyDatabufferWrite.length = 2;
    cyDatabufferWrite.buffer = input;
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

int Cls_EngineNQM::temperatureSensor(QString sort)
{
    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return false;
    }

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

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;

    CY_DATA_BUFFER cyDatabufferWrite, cyDatabufferRead;

    if(sort == "DMD")
    {
        unsigned char input[2], dmd[1];
        memset(input, 0, 2); memset(dmd, 0, 1);
        input[0] = (unsigned char)0x15;
        input[1] = (unsigned char)0x9C;
        cyDatabufferWrite.length = 2;
        cyDatabufferWrite.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf ("Error in doing i2c write \n");
            CyClose (cyHandle);

            return false;
        }

        cyDatabufferRead.length = 1;
        cyDatabufferRead.buffer = dmd;
        rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf ("Error in doing i2c write \n");
            CyClose (cyHandle);

            return -1;
        }

        int temper = cyDatabufferRead.buffer[0];

        return temper;

    }
    else if(sort == "LED")
    {
        unsigned char input[2], led[2];
        memset(input, 0, 2); memset(led, 0, 4);
        input[0] = (unsigned char)0x15;
        input[1] = (unsigned char)0x9F;
        cyDatabufferWrite.length = 2;
        cyDatabufferWrite.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        cyDatabufferRead.length = 4;
        cyDatabufferRead.buffer = led;
        rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return -1;
        }

        int temper = (cyDatabufferRead.buffer[3] << 8) + cyDatabufferRead.buffer[2]
                + (float)((cyDatabufferRead.buffer[1] << 8) + cyDatabufferRead.buffer[0]) / 65536;

        return temper;
    }
    else if(sort == "BOARD")
    {
        unsigned char input[2], board[2];
        memset(input, 0, 2); memset(board, 0, 2);
        input[0] = (unsigned char)0x15;
        input[1] = (unsigned char)0x9E;
        cyDatabufferWrite.length = 2;
        cyDatabufferWrite.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        cyDatabufferRead.length = 2;
        cyDatabufferRead.buffer = board;
        rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return -1;
        }

        int temper = ((cyDatabufferRead.buffer[1] & 0x0F) << 4) + ((cyDatabufferRead.buffer[0] & 0xF0) >> 4)
                + (float)(cyDatabufferRead.buffer[0] & 0x0F) / 16;

        return temper;
    }
    else { return false; }

    CyClose(cyHandle);

    return true;
}

int Cls_EngineNQM::lightSensor()
{
    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return false;
    }

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return -1;
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[2],rbuffer[2];
    memset(wbuffer, 0, 2);
    memset(rbuffer, 0, 2);

    wbuffer[0] = (unsigned char)0x15;
    wbuffer[1] = (unsigned char)0xF7;

    CY_DATA_BUFFER cyDatabufferWrite;
    cyDatabufferWrite.buffer = wbuffer;
    cyDatabufferWrite.length = 2;

    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return -1;
    }

    sleep(1); // Wait for 1 second, and then apply the I2C Read command next

    CY_DATA_BUFFER cyDatabufferRead;
    cyDatabufferRead.buffer = rbuffer;
    cyDatabufferRead.length = 2;
    rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return -1;
    }

    int light = (cyDatabufferRead.buffer[1] << 8) | cyDatabufferRead.buffer[0];

    CyClose(cyHandle);

    return light;
}

bool Cls_EngineNQM::read_GPIO1()
{
    /*Read GPIO-1 (I2C_BUSY) first. If its value is 0, which means the I2C bus is busy
    and we cannot send any I2C command now -- must retry it later; if its value is 1,
    then we can go on the following I2C command.*/

    int deviceNumber = FindDeviceAtSPIAndI2C("SPI");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("SPI Device open failed.\n");
        return false;
    }

    UINT8 gpioNum = 1; // LED_ON_OFF
    UINT8 value;

    rStatus = CyGetGpioValue(cyHandle, gpioNum, &value);

    if(rStatus != CY_SUCCESS)
    {
        printf ("Error in doing gpio write \n");
        CyClose (cyHandle);

        return false;
    }

    CyClose(cyHandle);

    return value;
}

bool Cls_EngineNQM::getUniformityMask()
{
    int addr = 0xA0000;
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
    status = FlashBlockRead(addr+8, png_len, png_buf);

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

bool Cls_EngineNQM::imageFlip(bool onOff)
{
    //Dlg_Message dlg;
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
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;
    unsigned char wbuffer[2];
    memset(wbuffer, 0, 2);
    wbuffer[0] = (unsigned char)0x1F;

    if(onOff) { wbuffer[1] = (unsigned char)0x01; }
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


CY_RETURN_STATUS Cls_EngineNQM::FlashBlockRead(int address, int len, char *buf)
{
    int BlockSize = 0x1000; // Block read 4KB size
    unsigned char wbuffer[0x1004], rbuffer[0x1004]; // array size is equal to BlockSize+4

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    CY_DATA_BUFFER cyDatabufferWrite, cyDatabufferRead;
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 1; // 1? 0?
    int cWriteSize, cReadSize;
    int str_addr = address;

    printf("Opening SPI device with device number %d...\n", deviceNumber);
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
        rStatus = CySpiReadWrite (cyHandle, &cyDatabufferRead, &cyDatabufferWrite, 5000);

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

CY_RETURN_STATUS Cls_EngineNQM::cySPIWaitForIdle(CY_HANDLE cyHandle)
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

    do
    {
        wr_data[0] = 0x05; /* Get SPI status */
        status =CySpiReadWrite (cyHandle,&readBuf, &writeBuf, 5000);

        if(status != CY_SUCCESS)
        {
            printf("\nFailed to send SPI status query command to device.");
            CyClose (cyHandle);

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

int Cls_EngineNQM::FindDeviceAtSPIAndI2C(QString device)
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
            if(deviceInfo.deviceType[0] == CY_TYPE_I2C)
            {
                return i;
            }
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

bool Cls_EngineNQM::showTestPatterns(QString sort) // Projector Source and Test Patterns
{
    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return false;
    }

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

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;

    if(sort == "HDMI")
    {
        unsigned char input[2];
        memset(input, 0, 2);
        CY_DATA_BUFFER cyDatabufferWrite;
        input[0] = (unsigned char)0x01;
        input[1] = (unsigned char)0x00;
        cyDatabufferWrite.length = 2;
        cyDatabufferWrite.buffer = input;
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
    else if(sort == "Ramp")
    {
        unsigned char input[2];
        memset(input, 0, 2);
        CY_DATA_BUFFER cyDatabufferWrite;
        input[0] = (unsigned char)0x01;
        input[1] = (unsigned char)0x01;
        cyDatabufferWrite.length = 2;
        cyDatabufferWrite.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        CY_DATA_BUFFER cyDatabufferWrite2;
        input[0] = (unsigned char)0x11;
        input[1] = (unsigned char)0x01;
        cyDatabufferWrite2.length = 2;
        cyDatabufferWrite2.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite2, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        CyClose(cyHandle);

        return true;
    }
    else if(sort == "ChckerBoard")
    {
        unsigned char input[2];
        memset(input, 0, 2);
        CY_DATA_BUFFER cyDatabufferWrite;
        input[0] = (unsigned char)0x01;
        input[1] = (unsigned char)0x01;
        cyDatabufferWrite.length = 2;
        cyDatabufferWrite.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        CY_DATA_BUFFER cyDatabufferWrite2;
        input[0] = (unsigned char)0x11;
        input[1] = (unsigned char)0x07;
        cyDatabufferWrite2.length = 2;
        cyDatabufferWrite2.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite2, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        CyClose(cyHandle);

        return true;
    }
    else if(sort == "SolidField")
    {
        unsigned char input[7];
        memset(input, 0, 7);
        CY_DATA_BUFFER cyDatabufferWrite;
        input[0] = (unsigned char)0x13;
        input[1] = (unsigned char)0xFF;
        input[2] = (unsigned char)0x03;
        input[3] = (unsigned char)0xFF;
        input[4] = (unsigned char)0x03;
        input[5] = (unsigned char)0xFF;
        input[6] = (unsigned char)0x03;
        cyDatabufferWrite.length = 7;
        cyDatabufferWrite.buffer = input;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        unsigned char input2[2];
        memset(input2, 0, 2);
        CY_DATA_BUFFER cyDatabufferWrite2;
        input2[0] = (unsigned char)0x01;
        input2[1] = (unsigned char)0x02;
        cyDatabufferWrite2.length = 2;
        cyDatabufferWrite2.buffer = input2;
        rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite2, 5000);

        if(rStatus != CY_SUCCESS)
        {
            printf("Error in doing i2c write \n");
            CyClose(cyHandle);

            return false;
        }

        CyClose(cyHandle);
        return true;
    }
    else
    {
        CyClose(cyHandle);
        return false;
    }
}

QString Cls_EngineNQM::getAppVersion()
{
    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return " ";
    }

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return " ";
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;

    unsigned char input[2];
    memset(input, 0, 2);
    CY_DATA_BUFFER cyDatabufferWrite;
    input[0] = (unsigned char)0x15;
    input[1] = (unsigned char)0x85;
    cyDatabufferWrite.length = 2;
    cyDatabufferWrite.buffer = input;
    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return " ";
    }

    usleep(100000);
    unsigned char output[4];
    memset(output, 0, 4);
    CY_DATA_BUFFER cyDatabufferRead;
    cyDatabufferRead.length = 4;
    cyDatabufferRead.buffer = output;
    rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return " ";
    }

    int m_major = cyDatabufferRead.buffer[3];
    int m_minor = cyDatabufferRead.buffer[2];
    int m_patch = (cyDatabufferRead.buffer[1] << 8) + cyDatabufferRead.buffer[0];
    QString version = QString::number(m_major) + "." + QString::number(m_minor) + "." + QString::number(m_patch);

    CyClose(cyHandle);
    return version;
}

QString Cls_EngineNQM::getSequenceRevision()
{
    bool ret = read_GPIO1();
    //Dlg_Message dlg;

    if(!ret)
    {
        //dlg.setMessage("English", tr("You may try again in a few seconds."), 3000);
        return " ";
    }

    int deviceNumber = FindDeviceAtSPIAndI2C("I2C");
    CY_RETURN_STATUS rStatus;
    CY_HANDLE cyHandle;
    int interfaceNum = 0;

    rStatus = CyOpen(deviceNumber, interfaceNum, &cyHandle);

    if(rStatus != CY_SUCCESS)
    {
        printf("i2c Device open failed.\n");
        return " ";
    }

    CY_I2C_DATA_CONFIG i2cDataConfig;
    i2cDataConfig.isStopBit = true;
    i2cDataConfig.slaveAddress = 0x1A;
    i2cDataConfig.isNakBit = true;

    unsigned char input[2];
    memset(input, 0, 2);
    CY_DATA_BUFFER cyDatabufferWrite;
    input[0] = (unsigned char)0x15;
    input[1] = (unsigned char)0xBE;
    cyDatabufferWrite.length = 2;
    cyDatabufferWrite.buffer = input;
    rStatus = CyI2cWrite(cyHandle, &i2cDataConfig, &cyDatabufferWrite, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return " ";
    }

    usleep(100000);
    unsigned char output[4];
    memset(output, 0, 4);
    CY_DATA_BUFFER cyDatabufferRead;
    cyDatabufferRead.length = 4;
    cyDatabufferRead.buffer = output;
    rStatus = CyI2cRead(cyHandle, &i2cDataConfig, &cyDatabufferRead, 5000);

    if(rStatus != CY_SUCCESS)
    {
        printf("Error in doing i2c write \n");
        CyClose(cyHandle);

        return " ";
    }

    QString Revision = "";

    CyClose(cyHandle);

    return Revision;
}
