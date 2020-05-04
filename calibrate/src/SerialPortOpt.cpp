//
// Created by sky on 17-1-10.
//

#include "SerialPortOpt.h"
#include "CommonOpt.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <iostream>

using namespace std;

#define SERIAL_PORT "/dev/ttyTHS1"

static int speed_arr[] = {B115200, B38400, B19200, B9600, B4800, B2400, B1200, B300, B57600};
static int name_arr[] = {115200, 38400, 19200, 9600, 4800, 2400, 1200, 300, 57600};

/*
 * 设置波特率
 *
 * */
static void set_speed(int fd, int speed)
{
	int i;
	int status;
	struct termios Opt;
	tcgetattr(fd, &Opt);
	for (i= 0; i < sizeof(speed_arr) / sizeof(int); i++) {
		if (speed == name_arr[i]) {
			tcflush(fd, TCIOFLUSH);
			cfsetispeed(&Opt, speed_arr[i]);
			cfsetospeed(&Opt, speed_arr[i]);
			status = tcsetattr(fd, TCSANOW, &Opt);
			if (status != 0) {
				perror("tcsetattr fd1");
				return;
			}
			tcflush(fd,TCIOFLUSH);
		}
	}
}

bool SerialPortOpt::init()
{
    // m_ifd = open("", O_RDWR | O_NOCTTY | O_NDELAY);
    m_ifd = open("/dev/ttyTHS1", O_RDWR | O_NOCTTY | O_NDELAY);
    if (m_ifd < 0) { /** error **/
        printf("[%s-%d] open error!!!\n", __FILE__, __LINE__);
        return false;
    }
    set_speed(m_ifd, 57600);
    m_bLinked = 1;
    fcntl(m_ifd, F_SETFL, 0);
}

SerialPortOpt::SerialPortOpt()
{
    m_iBaudrate = g_config.iBaudrate;
    m_ifd = 0;
    m_bLinked = 0;
    memset(m_strBuf, 0, 1024);
}

SerialPortOpt::~SerialPortOpt()
{
    if(m_bLinked)
    {
        close(m_ifd);
    }
}



/*
 * 接收串口数据
 * */
int SerialPortOpt::receiveData()
{
    int iRet;
    printf("start to receive...\n");
    iRet = read(m_ifd, m_strBuf, 1024);
    for(int i = 0; i < iRet; i++)
    {
        if((i & 0xf) == 0)
        {
            printf("\n");
        }
        printf("0x%02x ", m_strBuf[i]);
        // fflush( fflush(stdout));
    }

    return iRet;
}


/*
 * 发送字符串串口数据
 * */
int SerialPortOpt::sendData(char *strData, int iLen)
{
    int iRet;
    printf("start to sendData...\n");
    memset(m_strBuf, 0, 1024);
    memcpy(m_strBuf, strData, iLen);
    // memccpy(m_strBuf, strData, iLen);
    printf("send datalen = %d, data = %s\n", iLen, m_strBuf);
    iRet = write(m_ifd, m_strBuf, iLen);
    printf("send data length = %d\n", iRet);

    return iRet;
}

/*
 * 发送结构体串口数据
 * */
int SerialPortOpt::sendStructData(_st_serialport stSerialPort)
{
    int iRet;
    memset(m_strBuf, 0, 16);
    char time4[4];

    int cnt = 15;
    // header
    m_strBuf[0] = 0xA5;
    m_strBuf[1] = 0x5A;

    // header payload length
    m_strBuf[2] = 12;

    // payload
    // payload - time
    uint32_t iTime = stSerialPort.time1;
    m_strBuf[6] = (BYTE)(iTime&0xFF);
    iTime = iTime >> 8;
    m_strBuf[5] = (BYTE)(iTime&0xFF);
    iTime = iTime >> 8;
    m_strBuf[4] = (BYTE)(iTime&0xFF);
    iTime = iTime >> 8;
    m_strBuf[3] = (BYTE)(iTime&0xFF);

    // delta time
    m_strBuf[7] = stSerialPort.time2;
    // pass flag
    m_strBuf[8] = stSerialPort.bPass;

    // payload - x, y, z, 得到目的区域的中心三维坐标
    uint16_t iX = stSerialPort.x;
    uint16_t iY = stSerialPort.y;
    uint16_t iZ = stSerialPort.z;

    // payload -x
    m_strBuf[10] = (BYTE)(iX & 0xFF);
    iX = iX >> 8;
    m_strBuf[9] = (BYTE)(iX & 0xFF);

    // payload -y
    m_strBuf[12] = (BYTE)(iY & 0xFF);
    iY = iY >> 8;
    m_strBuf[11] = (BYTE)(iY & 0xFF);

    // payload -z
    m_strBuf[14] = (BYTE)(iZ & 0xFF);
    iZ = iZ >> 8;
    m_strBuf[13] = (BYTE)(iZ & 0xFF);

    // checksum
    long lSum = 0;
    for(int i = 0; i < 15; ++i)
    {
        lSum += m_strBuf[i];
    }

    m_strBuf[15] = (BYTE)(lSum);

    cnt += 1;
    iRet = write(m_ifd, m_strBuf, cnt);

    for(int i= 0; i < 16; ++i)
    {
        printf("0x%02x ", m_strBuf[i]);
    }
    printf("\n");


    // print struct content
    /*
     timestamp: 201586231
    timedelta: 117
    pass: 0
    x: 64767
    y: 64910
    z: 3120   */
    printf("time stamp = %d\n", stSerialPort.time1);
    printf("time delta = %d\n", stSerialPort.time2);
    printf("pass = %d\n", stSerialPort.bPass);
    printf("x = %d\n", (int)stSerialPort.x);
    printf("y = %d\n", (int)stSerialPort.y);
    printf("z = %d\n", (int)stSerialPort.z);

    /*
    cout << "time stamp = " << stSerialPort.time1 << endl;
    cout << "time delta = " << stSerialPort.time2 << endl;
    cout << "pass = " << stSerialPort.bPass << endl;
    cout << "x = " << stSerialPort.x << endl;
    cout << "y = " << stSerialPort.y << endl;
    cout << "z = " << stSerialPort.z << endl;
    */


    printf("send data length = %d\n", iRet);
    return iRet;

}
