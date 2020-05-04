#include "capVideo.h"
#include "CommonOpt.h"

#include <math.h>
#include <time.h>
#include <random>

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

capVideos::capVideos()
{
    m_iStereo1 = 1;
    m_iStereo2 = 2;
    m_iWid = 640;
    m_iHei = 480;
    m_iFPS = 30;
    m_iCameraCnt = 2;
    m_bInit = false;
    m_iCapture = 1;
    m_bWriteVideo = 0;
    m_strWriteVideo = "merge.avi";
    m_strPicXML = "./params/files.xml";
}


capVideos::capVideos(int iCameraPort1, int iCameraPort2, int iWid, int iHei, int iFPS, int iCameraCnt)
{
    m_iStereo1 = iCameraPort1;
    m_iStereo2 = iCameraPort2;
    m_iWid = iWid;
    m_iHei = iHei;
    m_iFPS = iFPS;
    m_iCameraCnt = iCameraCnt;
    m_bInit = false;
    m_iCapture = 1;
    m_bWriteVideo = 0;
    m_strWriteVideo = "merge.avi";
    m_strPicXML = "./params/files.xml";
}


capVideos::~capVideos()
{
    cap1.release();
    cap2.release();
    if(g_config.bWriteStitchVideo)
    {
        m_videoWrite.release();
    }

    if(g_config.bWriteVideo)
    {
        m_videoLeftW.release();
        m_videoRightW.release();
    }
}


void capVideos::setCameraPort(int iCameraPort1, int iCameraPort2)
{
    m_iStereo1 = iCameraPort1;
    m_iStereo2 = iCameraPort2;
}


void capVideos::setChannel(int iChannel)
{
    m_iChannel = iChannel;
}

void capVideos::setCameraCnt(int iCnt)
{
    m_iCameraCnt = iCnt;
}


void capVideos::setVideoFile(string strFile)
{
    m_strVideoFile = strFile;
}


void capVideos::setFPS(int iFPS)
{
    m_iFPS = iFPS;
}


void capVideos::setWidAndHei(int iWid, int iHei)
{
    m_iWid = iWid;
    m_iHei = iHei;
}


void capVideos::setShowSub(int iShowSub)
{
    m_iShowSub = iShowSub;
}


// void setCaptureMethod(int iMethod);
void capVideos::setCaptureMethod(int iMethod)
{
    m_iCapture = iMethod;

}

void capVideos::setWriteVideo(bool bWriteVideo)
{
    m_bWriteVideo = bWriteVideo;
}

void capVideos::setPicXMLPath(string strPath)
{
    m_strPicXML = strPath;
}


int capVideos::initCapStereoCamera()
{
    int iRe = 1;
    cap1.open(m_iStereo1);
    if (m_strVideoFile != "")
    {
        cap1.open(m_strVideoFile);
    }
    else
    {
        cout << "read default camera!" << endl;
        iRe = iRe & cap1.open(m_iStereo1);
    }

    if(!cap1.isOpened())
    {
        cout << "cap1 open failed!" << endl;
        return 0;
    }

    cap1.set(CV_CAP_PROP_FRAME_WIDTH, m_iWid);
    cap1.set(CV_CAP_PROP_FRAME_HEIGHT, m_iHei);

    cap1.set(CV_CAP_PROP_FPS, m_iFPS);

    if(m_iCameraCnt > 1)
    {
        cap2.open(m_iStereo2);
        if (m_strVideoFile != "")
        {
            cap2.open(m_strVideoFile);	// read a video file
        }
        else
        {
            cout << "read default camera!" << endl;
            iRe = iRe & cap2.open(m_iStereo2);	// read the default caera
        }

        if (!cap2.isOpened())// check if we succeeded
        {
            cout << "cap2 open failed!" << endl;
            return 0;
        }

        cap2.set(CV_CAP_PROP_FRAME_WIDTH, m_iWid);
        cap2.set(CV_CAP_PROP_FRAME_HEIGHT, m_iHei);


        cap2.set(CV_CAP_PROP_FPS, m_iFPS);
    }

    cout << "capture video opened success..." << endl;
    return iRe;

}

int capVideos::initWriteStitchCamera()
{
    if (!m_videoWrite.isOpened())
    {
        m_videoWrite.open(m_strWriteVideo, VideoWriter::fourcc('x','v','i','d'), 24,
                          Size(m_iWid * 3, m_iHei), true);
        if (!m_videoWrite.isOpened())
        {
            printf("can't create video writer\n");
            return 0;
        }
        // throw std::runtime_error("can't create video writer");
    }
}



int capVideos::initWriteCamera()
{
    int iRe = 1;
    char strTime[128];
    char strLeftVideo[128];
    char strRightVideo[128];

    memset(strTime, 0, 128);
    memset(strLeftVideo, 0, 128);
    memset(strRightVideo, 0, 128);

    // generate folder path
    time_t tt = time(NULL);//这句返回的只是一个时间cuo
    tm* t= localtime(&tt);
    int iRand = rand();
    sprintf(strTime, "%d-%02d-%02d-%02d-%02d-%02d-%d-%d-%d",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec, g_config.iVideoCnt, m_iWid, m_iHei);

    sprintf(strLeftVideo, "./%s-left.avi", strTime);
    sprintf(strRightVideo, "./%s-right.avi", strTime);

    if (!m_videoLeftW.isOpened())
    {
        iRe = iRe & m_videoLeftW.open(strLeftVideo, VideoWriter::fourcc('x','v','i','d'), 5,
                          Size(m_iWid, m_iHei), true);
        if (!m_videoLeftW.isOpened())
        {
            printf("can't create left video writer\n");
            return 0;
        }
        // throw std::runtime_error("can't create video writer");
    }

    if(g_config.iCameraCnt > 1)
    {
        if (!m_videoRightW.isOpened())
        {
            iRe = iRe & m_videoRightW.open(strRightVideo, VideoWriter::fourcc('x','v','i','d'), 5,
                               Size(m_iWid, m_iHei), true);
            if (!m_videoRightW.isOpened())
            {
                printf("can't create right video writer\n");
                return 0;
            }
            // throw std::runtime_error("can't create video writer");
        }
    }

    return iRe;
}

int capVideos::initCapture()
{
    int iRe = 1;
    if(g_config.iCapture == CAPTURE_VIDEO)
    {
        iRe = iRe & initCapStereoCamera();
    }

    // init write video capture
    if(g_config.bWriteStitchVideo)
    {
        iRe = iRe & initWriteStitchCamera();
    }


    // add by yyq [2017-0308] left, right video file name
    if(g_config.bWriteVideo)
    {
        iRe = iRe & initWriteCamera();
    }

    return iRe;
}

int capVideos::initReadVideo()
{
    // none
    printf("in read video mode!\n");
    // add by yyq [2017-0309]
    string strLeft = "./left.avi";
    string strRight = "./right.avi";
    cap1.open(strLeft);

    if(!cap1.isOpened())
    {
        cout << "cap1 open failed!" << endl;
        return 0;
    }

    cap1.set(CV_CAP_PROP_FRAME_WIDTH, m_iWid);
    cap1.set(CV_CAP_PROP_FRAME_HEIGHT, m_iHei);

    cap1.set(CV_CAP_PROP_FPS, m_iFPS);

//    Mat tmp;
//    cap1>>tmp;
//    imwrite("ss.jpg", tmp);

    cap2.open(strRight);

    if (!cap2.isOpened())// check if we succeeded
    {
        cout << "cap2 open failed!" << endl;
        return 0;
    }

    cap2.set(CV_CAP_PROP_FRAME_WIDTH, m_iWid);
    cap2.set(CV_CAP_PROP_FRAME_HEIGHT, m_iHei);


    cap2.set(CV_CAP_PROP_FPS, m_iFPS);

    cout << "read video opened success..." << endl;
    return 1;
}


int capVideos::initReadPic()
{
    readStringList(m_strPicXML, m_vecList);
    int iSize = m_vecList.size();
    if(iSize %3 != 0)
    {
        cout << "the file consturction is wrong" << endl;
        return 0;
    }
    return 1;
}


/*
 * init capVideos class
*/
int capVideos::init()
{
    int iRe = 1;
    switch(m_iCapture)
    {
        case CAPTURE_VIDEO:
        {
            iRe = iRe & initCapture();
            break;
        }
        case READ_VIDEO:
        {
            iRe = iRe & initReadVideo();
            break;
        }
        case READ_PIC:
        {
            iRe = iRe & initReadPic();
            break;
        }
        case WRITE_VIDEO:
        {
            iRe = iRe & initWriteVideo();
            break;
        }
        case WRITE_PIC:
        {
            iRe = iRe & initWritePic();
            break;
        }
        case READ_VIDEO_INTO_PIC:
        {
            iRe = iRe & initReadVideoIntoPic();
            break;
        }
        default:
        {
            iRe = iRe & initCapture();
            break;
        }
    }

    m_bInit = true;
    return iRe;
}

int capVideos::initReadVideoIntoPic()
{
    int iRe = 1;
    iRe = iRe & initReadVideo();
    return iRe;
}


int capVideos::catchLeftFrame(Mat &tmp)
{
    int iRe = 1;
    if(m_iChannel == 1)
    {
        cap1 >> tmp;
        if(tmp.rows > 0)
            cvtColor(tmp, frameL, COLOR_BGR2GRAY);
        if(tmp.rows <= 0)
            iRe = 0;
    }
    else
    {
        cap1 >> frameL;
        if(frameL.rows <= 0)
            iRe = 0;
    }

    return iRe;
}


int capVideos::catchRightFrame(Mat &tmp)
{
    int iRe = 1;
    if(m_iChannel == 1)
    {
        cap2 >> tmp;
        if(tmp.rows > 0)
            cvtColor(tmp, frameR, COLOR_BGR2GRAY);
        if(tmp.rows <= 0)
            iRe = 0;
    }
    else
    {
        cap2 >> frameR;
        if(frameR.rows <= 0)
            iRe = 0;
    }

    return iRe;
}


void capVideos::insertMat(Mat &matInsert)
{
    m_videoWrite << matInsert;
}


void capVideos::writeLeftMat(Mat &tmp)
{
    printf("write left frame ...");
    m_videoLeftW << tmp;
}

void capVideos::writeRightMat(Mat &tmp)
{
    printf("write right frame ...");
    m_videoRightW << tmp;
}

void capVideos::getStereoPic(string strLeft, string strRight)
{
    frameL = imread(strLeft, 1);
    frameR = imread(strRight, 1);
}



void capVideos::releaseVideo()
{
    if(g_config.bWriteStitchVideo)
    {
        m_videoWrite.release();
    }

    if(g_config.bWriteVideo)
    {
        m_videoLeftW.release();
        m_videoRightW.release();
    }
}


int capVideos::initWriteVideo()
{
    int iRe = initCapStereoCamera();
    iRe = iRe & initWriteCamera();
    return iRe;
}


int capVideos::initWritePic()
{
    int iRe = 1;
    initCapStereoCamera();
    return iRe;
}