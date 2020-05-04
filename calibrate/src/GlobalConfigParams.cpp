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

_st_config_param g_config;


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
                else if(strcmp(ptChParam, "SquareSize") == 0)
                {
                    config.fSquareSize = atof(ptChValue);
                }
                else if(strcmp(ptChParam, "Channel") == 0)    // Channel
                {
                    config.iChannel = atoi(ptChValue);
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
            }

        }
    }

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

    if (stat(config.strFolderPath, &st) == -1 && (g_config.iCapture == WRITEVIDEO || g_config.iCapture == WRITEPIC)) {
        mkdir(config.strFolderPath, 0777);
    }

    printf("folder path = %s\nsss\n", config.strFolderPath);
    sprintf(config.strOut_filename, "%s/example.txt", config.strFolderPath);
    printf("strOut_filename path = %s\nsss\n", config.strOut_filename);

    sprintf(config.strLeftVideo, "%s/left.avi", config.strFolderPath);
    printf("left video path = %s\nsss\n", config.strLeftVideo);

    sprintf(config.strRightVideo, "%s/right.avi", config.strFolderPath);
    printf("right video path = %s\nsss\n", config.strRightVideo);

    if(g_config.iCapture == CALIBRATE)
    {
        sprintf(config.strIntrinsic_filename, "%s-calib/intrinsics.yml", config.strFolderPath);

        printf("strIntrinsic_filename path = %s\nsss\n", config.strIntrinsic_filename);

        sprintf(config.strExtrinsic_filename, "%s-calib/extrinsics.yml", config.strFolderPath);

        printf("strExtrinsic_filename path = %s\nsss\n", config.strExtrinsic_filename);
    }



}
