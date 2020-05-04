//
// Created by sky on 17-1-5.
//

#ifndef _GLOBALPARAMS_H
#define _GLOBALPARAMS_H

#include "IncludeAll.h"

// get parameters from file
typedef struct _ST_CONFIG_PARAM
{
    int iCapture;
    int iCameraCnt;
    int iportl;
    int iportr;

    int iImgWid;
    int iImgHei;

    int iChannel;
	
	int iNr;
	
    int iAvoidMethod;

    bool bShowTime;
    bool bShowSub;
    bool bSharpen;
    bool bShowClrDisp;
    bool bWriteVideo;
    bool bWriteStitchVideo;
    bool bWriteLog;
    bool bShowPics;

    char strIntrinsic_filename[128];
    char strExtrinsic_filename[128];

    // algorithm parameters
    // alg type: 0-STEREO_BM; 1-STEREO_SGBM; 2-STEREO_HH; 3-STEREO_3WAY
    int iAlgType;

    int iDTSize;
    int iBlkSize;
    int iSADWindowSize;

    int iLleft;
    int iLright;
    int iLtop;
    int iLbottom;

    int iRleft;
    int iRright;

    int iEnlargeX;
    int iEnlargeY;

    float fRatio;
    float fFps;
    float fSquareSize;
    float falpha;
    float fsigma; // exp指数参数
    float fDotThresh; // 危险点数阈值

    int iDistThresh;
    int iMaxDistThresh;

    // computer type
    int iComType;

    // add by yyq 2017-0110
    // process mode iProcMode = 0: normal mode; 1:quick mode.
    int iProcMode;

    // add by yyq 2017-0110
    // add serial port configuration parameters
    bool bUseSerial;
    char strSerialPort[64];
    int iBaudrate;

    int iPlaneWid;
    int iPlaneHei;

    int iGridNum;
    int bDebug;

    int iPointNum;

    // add by yyq [2017-0309]
    int iVideoCnt;

    // add by yyq [2017-0313], video type = 0, normal camera; 1, zed camera
    int iVideoType;
	
    char strOut_filename[128];
    char strFolderPath[128];

    char strLeftVideo[128];
    char strRightVideo[128];

    int iSquareGridLen;
    int iSquareGridHei;
    int iSquareGridWid;


    _ST_CONFIG_PARAM()
    {
        iCapture = 0;
        iCameraCnt = 2;
        iportl = 0;
        iportr = 1;
        iImgWid = 640;
        iImgHei = 480;
		iNr = 0;
		bShowTime = 0;
        bShowSub = 0;
        bSharpen = 0;
        bShowClrDisp = 0;
        bWriteVideo = 0;
        bWriteStitchVideo = 0;
        bWriteLog = 0;
        bShowPics = 0;

        iLleft = 0;
        iLright = 0;
        iLtop = 0;
        iLbottom = 0;

        iRleft = 0;
        iRright = 0;

        iEnlargeX = 0;
        iEnlargeY = 0;

        memset(strIntrinsic_filename, 0, 128);
        memset(strExtrinsic_filename, 0, 128);

        strcpy(strIntrinsic_filename, "./params/intrinsics.yml");
        strcpy(strExtrinsic_filename, "./params/extrinsics.yml");
		memset(strOut_filename, 0, 128);
        memset(strFolderPath, 0, 128);

        memset(strLeftVideo, 0, 128);
        memset(strRightVideo, 0, 128);

        fSquareSize = 20.0;
        iSquareGridHei = 6;
        iSquareGridWid = 8;
        iSquareGridLen = 50;
		
		falpha = 0.5;

        iAlgType = 3;

        iDTSize = 48;
        fRatio = 2.8;
        iBlkSize = 9;
        iSADWindowSize = 9;

        fDotThresh = 0.11;
        fFps = 25.0;

        iDistThresh = 5000;
        iMaxDistThresh = 8000;

        iComType = 0;

        iChannel = 3;
        iAvoidMethod = 0;

        // add by yyq 2017-0110
        iProcMode = 0;

        bUseSerial = 0;
        memset(strSerialPort, 0, 64);
        iBaudrate = 0;

        iPlaneWid = 1000;
        iPlaneHei = 1000;
        bDebug = 1;
        iGridNum = 6;

        iPointNum = 10;
	    iVideoCnt = 0;
        iVideoType = 0;

    }
}_st_config_param;


/*
 * get parameters from params.ini
 */
extern void getParameters(_st_config_param &config);



#endif //_GLOBALPARAMS_H
