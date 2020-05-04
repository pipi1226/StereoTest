//
// Created by sky on 17-1-5.
//

#include "GlobalConfigParams.h"

#include <semaphore.h>
#include <math.h>
#include <time.h>
#include <random>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>



/*   删除左边的空格   */
char * del_left_trim(char *str)
{
    assert(str != NULL);
    for (;*str != '\0' && isblank(*str) ; ++str);
    return str;
}

/*   删除两边的空格   */
char * del_both_trim(char * str)
{
    char *p;
    char * szOutput;
    char params[128];
    memset(params, 0, 128);

    int iFlag = 0;
    int iInx = 0;

    szOutput = del_left_trim(str);
    for (p = szOutput + strlen(szOutput) - 1; p >= szOutput && isblank(*p); --p);
    *(++p) = '\0';

    return szOutput;
}

void writeParameters(_st_config_param &config)
{
    FILE * fp = NULL;
    /*打开配置文件a.txt*/

    const char* strfile = "./params/camera.ini";
    fp = fopen(strfile, "r");

    printf("writeParameters() ... file name = %s\n", strfile);
    if(fp == NULL)
    {
        printf("open ini file failed..\n");
        exit(0);
    }

    /*緩沖區*/
    char buf[64];
    char s[64];
    /*分割符*/
    const char * delim = "=";
    char * p;
    char ch;
    while (!feof(fp))
    {
        if ((p = fgets(buf, sizeof(buf), fp)) != NULL)
        {
            strcpy(s, p);
            ch=del_left_trim(s)[0];
            /*判断注释 空行，如果是就直接下次循环*/
            if (ch == '#' || isblank(ch) || ch=='\n')
                continue;

            /*分割字符串*/
            char *ptChParam = NULL;
            char *ptChValue = NULL;

            p=strtok(s, delim);
            if(p)
            {
                ptChParam = del_both_trim(p);
                printf("%s", ptChParam);

                while ((p = strtok(NULL, delim)))
                {
                    ptChValue = del_both_trim(p);
                    printf("%s ", ptChValue);
                }
               
		if(strcmp(ptChParam, "VideoCnt") == 0)   // VideoCnt
                {
                    config.iVideoCnt = atoi(ptChValue);
		    config.iVideoCnt += 1;		    
		    break;
                }
            }

        }
    }


    fclose(fp);

    printf("get video cnt params\n");

    fp = fopen(strfile, "w");

    printf("file name = %s\n", strfile);
    if(fp == NULL)
    {
        printf("open ini file failed..\n");
        exit(0);
    }

    while (!feof(fp))
    {
        printf("write camera.ini");
        fprintf(fp, "VideoCnt=%d", config.iVideoCnt);
	break;
    }


    fclose(fp);

}

// get global configuration parameters
void getParameters(_st_config_param &config)
{

    FILE * fp = NULL;
    /*打开配置文件a.txt*/

    const char* strfile = "./params/params.ini";
    fp = fopen(strfile, "r");

    printf("file name = %s\n", strfile);
    if(fp == NULL)
    {
        printf("open ini file failed..\n");
        exit(0);
    }

    /*緩沖區*/
    char buf[64];
    char s[64];
    /*分割符*/
    const char * delim = "=";
    char * p;
    char ch;
    while (!feof(fp))
    {
        if ((p = fgets(buf, sizeof(buf), fp)) != NULL)
        {
            strcpy(s, p);
            ch=del_left_trim(s)[0];
            /*判断注释 空行，如果是就直接下次循环*/
            if (ch == '#' || isblank(ch) || ch=='\n')
                continue;

            /*分割字符串*/
            char *ptChParam = NULL;
            char *ptChValue = NULL;

            p=strtok(s, delim);
            if(p)
            {
                ptChParam = del_both_trim(p);
                printf("%s", ptChParam);

                while ((p = strtok(NULL, delim)))
                {
                    ptChValue = del_both_trim(p);
                    printf("%s ", ptChValue);
                }

                if(strcmp(ptChParam, "portl") == 0)
                {
                    config.iportl = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "portr") == 0)
                {
                    config.iportr = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "ImgWid") == 0)
                {
                    config.iImgWid = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "ImgHei") == 0)
                {
                    config.iImgHei= atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "ShowTime") == 0)
                {
                    config.bShowTime = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "SquareSize") == 0)
                {
                    config.fSquareSize = atof(ptChValue);
                }
                else if(strcmp(ptChParam, "AlgType") == 0)
                {
                    config.iAlgType = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "DTSize") == 0)
                {
                    config.iDTSize = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "Ratio") == 0)
                {
                    config.fRatio = atof(ptChValue);
                }
                else if(strcmp(ptChParam, "BlkSize") == 0)
                {
                    config.iBlkSize = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "fps") == 0)
                {
                    config.fFps = atof(ptChValue);
                    if(config.fFps - 1.0e-5 < 1.0e-5)
                        config.fFps = 3.0;
                }
                else if(strcmp(ptChParam, "ShowSub") == 0)
                {
                    config.bShowSub = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "SADWindowSize") == 0)
                {
                    config.iSADWindowSize = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "Sharpen") == 0)
                {
                    config.bSharpen = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "CameraCnt") == 0)
                {
                    config.iCameraCnt = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "DistThresh") == 0)    // iDistThresh
                {
                    config.iDistThresh = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "ComType") == 0)    // ComType
                {
                    config.iComType = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "Alpha") == 0)    // Alpha
                {
                    config.falpha = atof(ptChValue);
                }
                else if(strcmp(ptChParam, "Channel") == 0)    // Channel
                {
                    config.iChannel = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "AvoidMethod") == 0)    // AvoidMethod
                {
                    config.iAvoidMethod = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "ShowClrDisp") == 0)    // bShowClrDisp
                {
                    config.bShowClrDisp = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "WriteVideo") == 0)    // bWriteVideo
                {
                    config.bWriteVideo = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "WriteStitchVideo") == 0)    // bWriteStitchVideo
                {
                    config.bWriteStitchVideo = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "WriteLog") == 0)    // bWriteLog
                {
                    config.bWriteLog = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "Capture") == 0)    // Capture
                {
                    config.iCapture = atoi(ptChValue);
                }
				else if(strcmp(ptChParam, "Intrinsic_filename") == 0)
                {
                    // config.iSerialPort = atoi(ptChValue);
                    sprintf(config.strIntrinsic_filename, "%s", ptChValue);
                }
                else if(strcmp(ptChParam, "Extrinsic_filename") == 0)
                {
                    // config.iSerialPort = atoi(ptChValue);
                    sprintf(config.strExtrinsic_filename, "%s", ptChValue);
                }
                else if(strcmp(ptChParam, "leftVideo") == 0)
                {
                    // config.iSerialPort = atoi(ptChValue);
                    sprintf(config.strExtrinsic_filename, "%s", ptChValue);
                }
                else if(strcmp(ptChParam, "rightVideo") == 0)
                {
                    // config.iSerialPort = atoi(ptChValue);
                    sprintf(config.strExtrinsic_filename, "%s", ptChValue);
                }
                else if(strcmp(ptChParam, "nr") == 0)
                {
                    config.iNr = atoi(ptChValue);
                }
		        else if(strcmp(ptChParam, "SquareGridLen") == 0) // SquareGridLen
                {
                    config.iSquareGridLen = atoi(ptChValue);
                }
		        else if(strcmp(ptChParam, "SquareGridHei") == 0) //SquareGridHei
                {
                    config.iSquareGridHei = atoi(ptChValue);
                }
		        else if(strcmp(ptChParam, "SquareGridWid") == 0) //SquareGridWid
                {
                    config.iSquareGridWid = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "Sigma") == 0)    // fsigma
                {
                    config.fsigma = atof(ptChValue);
                }
                else if(strcmp(ptChParam, "DotThresh") == 0)    // DotThresh
                {
                    config.fDotThresh = atof(ptChValue);
                }
                else if(strcmp(ptChParam, "EnlargeX") == 0)    // EnlargeX
                {
                    config.iEnlargeX = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "EnlargeY") == 0)    // EnlargeY
                {
                    config.iEnlargeY = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "ProcMode") == 0)    // iProcMode
                {
                    config.iProcMode = atoi(ptChValue);
                }                
                else if(strcmp(ptChParam, "UseSerial") == 0)   // bUseSerial
                {
                    config.bUseSerial = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "Baudrate") == 0)   // iBaudrate
                {
                    config.iBaudrate = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "PlaneWid") == 0)   // PlaneWid
                {
                    config.iPlaneWid = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "PlaneHei") == 0)   // PlaneHei
                {
                    config.iPlaneHei = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "GridNum") == 0)   // GridNUm
                {
                    config.iGridNum = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "Debug") == 0)   // Debug
                {
                    config.bDebug = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "PointNum") == 0)   // PointNum
                {
                    config.iPointNum = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "MaxDistThresh") == 0)   // MaxDistThresh
                {
                    config.iMaxDistThresh = atoi(ptChValue);
                }
                else if(strcmp(ptChParam, "VideoType") == 0)   // VideoType
                {
                    config.iVideoType = atoi(ptChValue);
                }
		
            }

        }
    }

    fclose(fp);

    // write camera cnt
    if(config.bWriteVideo)
        writeParameters(config);

    config.iLtop = int((config.iImgHei / 2) - (config.iImgHei / (2*2.8)));
    config.iLbottom = int((config.iImgHei / 2) + (config.iImgHei / (2*2.8)));

    // left picture rectangle
    config.iLleft = int((config.iImgWid - config.iDTSize) / 2 - (config.iImgWid - config.iDTSize) / (2 * config.fRatio) + config.iDTSize);
    config.iLright = int(((config.iImgWid - config.iDTSize) / 2 + (config.iImgWid - config.iDTSize) / (2 * config.fRatio) + config.iDTSize));

    // right picture rectangle
    // ((W - DisT) / 2 - (W - DisT) / (2 * ratio)), ((W - DisT) / 2 + (W - DisT) / (2 * ratio))
    config.iRleft = int((config.iImgWid - config.iDTSize) / 2 - (config.iImgWid - config.iDTSize) / (2 * config.fRatio));
    config.iRright = int((config.iImgWid - config.iDTSize) / 2 + (config.iImgWid - config.iDTSize) / (2 * config.fRatio));

    printf("global parameters:-------------------------\n");
    printf("leftusb = %d, rightusb = %d\n", config.iportl, config.iportr);
    printf("imgwid = %d, imghei = %d\n", config.iImgWid, config.iImgHei);

    printf("subleftl = %d, subleftr = %d\n", config.iLleft, config.iLright);
    printf("subleftt = %d, subleftb = %d\n", config.iLtop, config.iLbottom);

    printf("subrightl = %d, subrightr = %d\n", config.iRleft, config.iRright);

    printf("showtime = %d, squaresize = %lf\n", config.bShowTime, config.fSquareSize);
    printf("algtype = %d, DTSize = %d\n", config.iAlgType, config.iDTSize);
    printf("Ratio = %lf, BlkSize = %d\n", config.fRatio, config.iBlkSize);
    printf("fps = %f\n", config.fFps);
    printf("Sharpen = %d\n", config.bSharpen);
    printf("SADWndSize = %d\n", config.iSADWindowSize);
    printf("camera cnt = %d\n", config.iCameraCnt);
    printf("computer type = %d\n", config.iComType);
    printf("computer serial port = %s\n", config.strSerialPort);
	
	printf("\n");
	
	// generate folder path
    time_t tt = time(NULL);//这句返回的只是一个时间cuo
    tm* t= localtime(&tt);
    int iRand = rand();
    sprintf(config.strFolderPath, "%d-%02d-%02d-%02d-%02d-%02d-%d-%d",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec, config.iImgWid, config.iImgHei);

    struct stat st = {0};

    if (stat(config.strFolderPath, &st) == -1 &&
            (config.iCapture == WRITE_VIDEO ||
             config.iCapture == WRITE_PIC ||
             config.iCapture == READ_VIDEO_INTO_PIC
            )) {
        mkdir(config.strFolderPath, 0777);
    }

    printf("folder path = %s\nsss\n", config.strFolderPath);
    sprintf(config.strOut_filename, "%s/example.txt", config.strFolderPath);
    printf("strOut_filename path = %s\nsss\n", config.strOut_filename);

    sprintf(config.strLeftVideo, "%s/left.avi", config.strFolderPath);
    printf("left video path = %s\nsss\n", config.strLeftVideo);

    sprintf(config.strRightVideo, "%s/right.avi", config.strFolderPath);
    printf("right video path = %s\nsss\n", config.strRightVideo);

    if(config.iCapture == CALIBRATE)
    {
        sprintf(config.strIntrinsic_filename, "%s-calib/intrinsics.yml", config.strFolderPath);

        printf("strIntrinsic_filename path = %s\nsss\n", config.strIntrinsic_filename);

        sprintf(config.strExtrinsic_filename, "%s-calib/extrinsics.yml", config.strFolderPath);

        printf("strExtrinsic_filename path = %s\nsss\n", config.strExtrinsic_filename);
    }


}
