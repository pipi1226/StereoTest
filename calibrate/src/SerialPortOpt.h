//
// Created by sky on 17-1-10.
//

#ifndef DEMO_DIST_VFH_COLOR_SERIALPORTOPT_H
#define DEMO_DIST_VFH_COLOR_SERIALPORTOPT_H

// #include <FreeImage.h>
 #include <stdint.h>

typedef uint8_t BYTE;

typedef struct _ST_SERIRALPORT
{

    uint32_t time1;
    uint8_t time2;
    uint8_t bPass;
    int16_t x, y, z;

    _ST_SERIRALPORT()
    {
        time1 = 0;
        time2 = 0;
        bPass = 0;
        x = 0;
        y = 0;
        z = 0;
    }
}_st_serialport;


class SerialPortOpt
{
public:
    SerialPortOpt();

    virtual ~SerialPortOpt();
    bool init();
    int sendData(char *strData, int iLen);
    int sendStructData(_st_serialport stSerialPort);
    int receiveData();

private:
    int m_iPort;
    int m_iBaudrate;
    int m_ifd; /* File descriptor for the port */
    uint8_t m_strBuf[1024];
    bool m_bLinked;
};

#endif //DEMO_DIST_VFH_COLOR_SERIALPORTOPT_H
