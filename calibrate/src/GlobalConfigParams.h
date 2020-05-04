//
// Created by sky on 17-1-5.
//

#ifndef _GLOBALPARAMS_H
#define _GLOBALPARAMS_H

#include "IncludeAll.h"

typedef enum {
    CAPVIDEO = 0,
    READVIDEO = 1,
    READPIC = 2,
    WRITEVIDEO = 3,
    WRITEPIC = 4,
    CALIBRATE = 5
} USAGE_METHOD;

// get parameters from file
typedef struct _ST_CONFIG_PARAM
{
    int iCapture;
    int iportl;
    int iportr;

    int iImgWid;
    int iImgHei;

    int iChannel;

    int iNr;

    char strIntrinsic_filename[128];
    char strExtrinsic_filename[128];
    char strOut_filename[128];
    char strFolderPath[128];

    char strLeftVideo[128];
    char strRightVideo[128];

    int iSquareGridLen;
    int iSquareGridHei;
    int iSquareGridWid;
    float fSquareSize;

    _ST_CONFIG_PARAM()
    {
        iCapture = 0;
        iportl = 0;
        iportr = 1;
        iImgWid = 640;
        iImgHei = 480;
        iNr = 0;

        memset(strLeftVideo, 0, 64);
        memset(strRightVideo, 0, 64);
        memset(strIntrinsic_filename, 0, 128);
        memset(strExtrinsic_filename, 0, 128);
        memset(strOut_filename, 0, 128);
        memset(strFolderPath, 0, 128);

        memset(strLeftVideo, 0, 128);
        memset(strRightVideo, 0, 128);

        fSquareSize = 20.0;
        iSquareGridHei = 6;
        iSquareGridWid = 8;
        iSquareGridLen = 50;

    }
}_st_config_param;

extern _st_config_param g_config;


/*
 * get parameters from params.ini
 */
extern void getParameters(_st_config_param &config);



#endif //_GLOBALPARAMS_H
